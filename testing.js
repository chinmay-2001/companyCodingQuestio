import { DraftOrder, PaymentStatus, Refund, TransactionBaseService } from "@medusajs/medusa"
import { dataSource } from "@medusajs/medusa/dist/loaders/database"
import OrderRepository from "@medusajs/medusa/dist/repositories/order"
import PaymentRepository from "@medusajs/medusa/dist/repositories/payment"
import { Lifetime } from "awilix"
import { differenceInDays, endOfDay, format, parseISO, startOfDay, subDays, subMilliseconds } from "date-fns"
import { Brackets, IsNull, SelectQueryBuilder } from "typeorm"

import { TopProduct } from "../api/routes/admin/reports/list-top-products"
import { StoreOrderStats } from "../api/routes/admin/reports/store-order-stats"
import { OrderTimeseries } from "../api/routes/crm/order-metrics"
import { Cart } from "../models/cart"; // importing Cart model for Conversion Rate query builder
import { GiftCard } from "../models/gift-card"
import { LineItem } from "../models/line-item"
import { Order } from "../models/order"
import { Payment } from "../models/payment"
import { PaymentLog } from "../models/payment-log"
import { Product } from "../models/product"
import { ProductPage } from "../models/product-page"
import { ProductVariant } from "../models/product-variant"
import { Region } from "../models/region"
import { User } from "../models/user"
import ProductRepository from "../repositories/product"
import { getTimeFrame } from "../utils/commons"
import { DATE_FORMATS, ORDER_PURCHASE_OPTIONS_ENUM, PAYMENT_STATUS_TO_INCLUDE_IN_REVENUE, REPORTS_ORDER_TYPE_ENUM } from "../utils/constants"
export interface ReportOrders extends Order {
  order_item_count?: number
}

export interface FindRecentOrdersConfig {
  store_id: string
  offset: number
  limit: number
  include_orders_count?: boolean
  include_failed_payment_count?: boolean
}

export type TimeFrame = "hour" | "day" | "week" | "month"
export interface StoreOrderStatsParams {
  start_date: string
  end_date: string
  type?: string
  products?: string[]
  store_id?: string
  created_by?: string
}

export interface FindNewCustomersStatsConfig {
  start_date: string
  end_date: string
}

interface FindTopProductsConfig {
  start_date: string
  end_date: string
  type?: string,
}

interface RevenuePerProductInput {
  store_id: string
  start_date: string
  end_date: string
  type?: string
  user_ids?: string // defining user_ids as new parameter in this api to get data on the basis of user ids shared
}

interface TopProducts {
  id: string
  title: string
  revenue: number
}

interface ProductsTimeSeriesData {
  product_id: string
  start_date: Date
  end_date: Date
  revenue: number
}

interface RevenuePerProduct {
  productId: string
  productName: string
  amount: number
  timeseries: {
    start: string
    end: string
    amount: number
  }[]
}

/**
 * Converts an ISO date string to the UTC format (YYYY-MM-DDTHH:mm:ss.sssZ),
 * setting the time portion based on whether the date is an end date or start date.
 * 
 * @param isoDateStr - The ISO date string to convert.
 * @param isEndDate - If true, sets the time to the end of the day (23:59:59.999); 
 *                    otherwise, sets it to the start of the day (00:00:00.000).
 * @returns The date in UTC format (YYYY-MM-DDTHH:mm:ss.sssZ).
 */
const convertISOToUTCFormat = (isoDateStr: string, isEndDate: boolean = false): string => {
  // Parse the ISO date string into a Date object
  const dateObj = new Date(isoDateStr);

  // Extract UTC year, month, and day components
  const year = dateObj.getUTCFullYear();
  const month = String(dateObj.getUTCMonth() + 1).padStart(2, '0');  // Zero-padded month
  const day = String(dateObj.getUTCDate()).padStart(2, '0');         // Zero-padded day

  // Set time portion based on whether it's the start or end of the day
  const time = isEndDate ? '23:59:59.999' : '00:00:00.000';

  // Format and return the date in UTC as YYYY-MM-DDTHH:mm:ss.sssZ
  return `${year}-${month}-${day}T${time}Z`;
};

/**
 * Extracts the date portion from a datetime string, removing the timestamp.
 *
 * @param dateStr - A string with date and time (e.g., "2023-10-15 14:30:00").
 * @returns The date in YYYY-MM-DD format.
 */
function removeTimestamp(dateStr: string): string {
  // Split by space and return only the date portion (first element)
  return dateStr.split(' ')[0];
}

/**
 * Determines the correct format for the end date based on comparison with the start date.
 * If the start and end dates match in date portion, the end date is converted to UTC format 
 * with time set to the end of the day (23:59:59.999). Otherwise, the original end date is returned.
 *
 * @param paramStartDate - The start date in ISO format.
 * @param paramEndDate - The end date in ISO format.
 * @returns The adjusted end date in UTC format if start and end dates match; otherwise, the original end date.
 */
const queryEndDate = (paramStartDate: string, paramEndDate: string): string => {
  // Check if the date portions of start and end dates are the same
  const isEndDateMatch = removeTimestamp(paramStartDate) === removeTimestamp(paramEndDate);
  
  // If dates match, return the end date in UTC format set to end of day; else, return original end date
  return isEndDateMatch
    ? convertISOToUTCFormat(paramEndDate, true)  // Convert to UTC end of day
    : paramEndDate;
};



class ReportsService extends TransactionBaseService {
  store_id: string;
  static LIFE_TIME = Lifetime.SCOPED
  protected readonly loggedInUser_: User | null
  protected readonly orderRepository_: typeof OrderRepository
  protected readonly paymentRepository_: typeof PaymentRepository

  constructor(container) {
    super(container)
    try {
      this.loggedInUser_ = container.loggedInUser
      this.orderRepository_ = container.orderRepository
      this.paymentRepository_ = container.paymentRepository
    } catch (e) {
      // avoid errors when backend first runs
    }
  }

  /**
   * @description Retieves the store order stats with the given filters
   * @param params StoreOrderStatsParams
   * @returns Promise<StoreOrderStats>
   */
  async storeOrderStats(params: StoreOrderStatsParams): Promise<StoreOrderStats> {
    const { start_date, end_date, type, products , store_id} = params
    
    // handled the case when store_id is not provided in the params
    this.store_id = this.loggedInUser_?.store_id || store_id;

    const startDate = format(startOfDay(parseISO(start_date)), DATE_FORMATS.iso8601)
    const endDate = format(endOfDay(parseISO(end_date)), DATE_FORMATS.iso8601)

    const startDateObj = parseISO(startDate)
    const endDateObj = parseISO(endDate)

    const durationInDays = differenceInDays(endDateObj, startDateObj) + 1
    const timeFrame = getTimeFrame(parseISO(start_date), parseISO(end_date))

    const previousStartDateObj = subDays(startDateObj, durationInDays)
    const previousEndDateObj = subDays(endDateObj, durationInDays)

    const previousStartDate = format(previousStartDateObj, DATE_FORMATS.iso8601)
    const previousEndDate = format(previousEndDateObj, DATE_FORMATS.iso8601)


  /**
   * add convert time stamp fuction in gobal scope 
   */
    // sub-query to use in the time-series calculations

    const previousRefundLineItemSubquery = this.manager_.createQueryBuilder()
      .select([
        "o.id AS order_id",
        "MAX(r.created_at) AS created_at",
        `CASE
          WHEN o.payment_status = '${PaymentStatus.PARTIALLY_REFUNDED}'
            THEN COALESCE((li.metadata->>'refunded_amount')::numeric, 0)
          ELSE (CAST(li.quantity AS NUMERIC) * CAST(li.unit_price AS NUMERIC))
        END AS refund_amount`,
        "o.payment_status",
        "li.metadata"
      ])
      .from(LineItem, "li")
      .innerJoin(Order, "o", "li.order_id = o.id")
      .innerJoin(ProductVariant, "pv", "pv.id = li.variant_id")
      .innerJoin(Product, "p", "p.id = pv.product_id")
      .leftJoin(DraftOrder, "dro", "dro.order_id = o.id")
      .innerJoin(Refund, "r", "r.order_id = o.id")
      .where("o.store_id = :store_id", { store_id: this.store_id })
      .andWhere("o.payment_status IN (:...statuses)", { statuses: [PaymentStatus.PARTIALLY_REFUNDED, PaymentStatus.REFUNDED] })
      .andWhere("r.created_at >= :start_date AND r.created_at <= :end_date", {
        start_date: previousStartDate,
        end_date: previousEndDate
      })
      .groupBy("o.id")
      .addGroupBy("li.metadata")
      .addGroupBy("li.quantity")
      .addGroupBy("li.unit_price")
      .addGroupBy("o.payment_status");

    const refundLineItemSubquery = this.manager_.createQueryBuilder()
      .select([
        "o.id AS order_id",
        "MAX(r.created_at) AS created_at",
        `CASE
          WHEN o.payment_status = '${PaymentStatus.PARTIALLY_REFUNDED}'
            THEN COALESCE((li.metadata->>'refunded_amount')::numeric, 0)
          ELSE (
            (CAST(li.quantity AS NUMERIC) * CAST(li.unit_price AS NUMERIC))
            - COALESCE(gc_total.total_gc_value / NULLIF(line_item_count.count, 0), 0)
            - COALESCE(discount_total.total_discount / NULLIF(line_item_count.count, 0), 0)
          )
        END AS refund_amount`,
        "o.payment_status",
        "li.metadata"
      ])
      .from(LineItem, "li")
      .innerJoin(Order, "o", "li.order_id = o.id")
      .innerJoin(ProductVariant, "pv", "pv.id = li.variant_id")
      .innerJoin(Product, "p", "p.id = pv.product_id") 
      .leftJoin(DraftOrder, "dro", "dro.order_id = o.id")
      .innerJoin(Refund, "r", "r.order_id = o.id")
      // Join gift card total per order
      .leftJoin(
        qb => qb
          .select("li_gc.order_id", "order_id")
          .addSelect("SUM(gc.value) AS total_gc_value")
          .from(LineItem, "li_gc")
          .innerJoin("gift_card", "gc", "gc.order_id = li_gc.order_id")
          .groupBy("li_gc.order_id"),
        "gc_total",
        "gc_total.order_id = o.id"
      )
      // Join discount total per order
      .leftJoin(
        qb => qb
          .select("li_disc.order_id", "order_id")
          .addSelect("SUM(CASE WHEN dr.type = 'percentage' THEN li_disc.unit_price * (dr.value / 100) * li_disc.quantity ELSE dr.value * li_disc.quantity END) AS total_discount")
          .from(LineItem, "li_disc")
          .innerJoin("order_discounts", "od", "od.order_id = li_disc.order_id")
          .innerJoin("discount", "d", "od.discount_id = d.id")
          .innerJoin("discount_rule", "dr", "d.rule_id = dr.id")
          .groupBy("li_disc.order_id"),
        "discount_total",
        "discount_total.order_id = o.id"
      )
      // Join line item count per order
      .leftJoin(
        qb => qb
          .select("li_count.order_id", "order_id")
          .addSelect("COUNT(*) AS count")
          .from(LineItem, "li_count")
          .groupBy("li_count.order_id"),
        "line_item_count",
        "line_item_count.order_id = o.id"
      )
      .where("o.store_id = :store_id", { store_id: this.store_id })
      .andWhere("o.payment_status IN (:...statuses)", { statuses: [PaymentStatus.PARTIALLY_REFUNDED, PaymentStatus.REFUNDED] })
      .andWhere("r.created_at >= :start_date AND r.created_at <= :end_date", {
        start_date: startDate,
        end_date: endDate
      })
      .groupBy("o.id")
      .addGroupBy("li.metadata")
      .addGroupBy("li.quantity")
      .addGroupBy("li.unit_price")
      .addGroupBy("o.payment_status")
      .addGroupBy("gc_total.total_gc_value")     
      .addGroupBy("discount_total.total_discount")
      .addGroupBy("line_item_count.count");       
  
    const subqueryForTimeSeries = this.manager_.createQueryBuilder()
      .select([
        "li.quantity AS quantity",
        "li.unit_price AS unit_price",
        "o.id AS order_id",
        "o.created_at AS created_at",
        "o.payment_status AS payment_status",
        "li.metadata AS metadata"
      ])
      .from(LineItem, "li")
      .innerJoin(Order, "o", "li.order_id = o.id AND o.store_id = :store_id AND payment_status IN (:...payment_status)")
      .innerJoin(ProductVariant, "pv", "pv.id = li.variant_id")
      .innerJoin(Product, "p", "p.id = pv.product_id")
      .leftJoin(DraftOrder, "dro", "dro.order_id = o.id")

      const voidQueryBuilder = this.manager_
        .createQueryBuilder()
        .select([
          "li.quantity AS quantity",
          "li.unit_price AS unit_price",
          "o.id AS order_id",
          "o.updated_at AS updated_at",
          "o.payment_status AS payment_status",
          "li.metadata AS metadata"
        ])
        .from(LineItem, "li")
        .innerJoin(Order, "o", "li.order_id = o.id AND o.store_id = :store_id AND o.payment_status IN (:...payment_status)", {
          store_id: this.store_id,
          payment_status: [PaymentStatus.VOIDED],
        })
        .innerJoin(ProductVariant, "pv", "pv.id = li.variant_id")
        .innerJoin(Product, "p", "p.id = pv.product_id")
        .leftJoin(DraftOrder, "dro", "dro.order_id = o.id");



    /**
     * Creates a subquery to retrieve time series data for page views.
     * The query selects relevant data from the Cart and ProductPage tables,
     * filtering out any deleted cart entries.
     *
     * @returns {SelectQueryBuilder<Cart>} - A query builder instance configured to fetch
     *                                        time series data for page views.
     */
    const subqueryForViewsTimeSeries = this.manager_.createQueryBuilder()
      .select([
      "c.metadata->>'pageId' AS page_id", // Extract the page ID from cart metadata
      "c.id AS cart_id",  // Select the cart ID
      "c.created_at AS created_at"  // Select the creation timestamp of the cart
      ])
      .from(Cart, "c")  // Specify the Cart table as the source
      // .leftJoin(ProductPage, "pp", "pp.id = c.metadata->>'pageId'")  // Join with ProductPage based on the extracted page ID
      .leftJoin(LineItem, "li", "li.cart_id = c.id")
      .leftJoin(ProductVariant, "pv", "li.variant_id = pv.id") // Add join to ProductVariant
      .leftJoin(Product, "p", "p.id = pv.product_id") // Add join to Product
      .leftJoin(Region, "r", "r.id = c.region_id")  // Join with Region based on the region ID
      .where("r.store_id = :store_id", { store_id: this.store_id })
      .andWhere("c.created_at::timestamp >= :start_date AND c.created_at::timestamp <= :end_date", {
      start_date: startDate, // Parameter for the start date
      end_date: queryEndDate(startDate, endDate) // Calculate the end date based on the input range
      })
      .andWhere(new Brackets(qb => {
      qb.where("li.metadata->>'is_refunded' IS NULL").orWhere("li.metadata->>'is_refunded' = 'false'"); // Ensure carts are not refunded
      }))
      // Only count carts that have at least one associated order
      .andWhere(`EXISTS (SELECT 1 FROM "order" o WHERE o.cart_id = c.id)`)

      if (type === 'upsell' && products && products.length > 0) {
        subqueryForViewsTimeSeries.andWhere(`
          EXISTS (
            SELECT 1 FROM line_item li2
            WHERE li2.cart_id = c.id
              AND li2.metadata->>'is_upsell' = 'true'
              AND li2.variant_id IN (
                SELECT id FROM product_variant WHERE product_id IN (:...products)
              )
          )
        `, { products });
      }

    //   .andWhere(`
    //   NOT EXISTS (
    //     SELECT 1 FROM "order" o
    //     WHERE o.cart_id = c.id
    //       AND o.metadata->>'is_virtual_terminal_order' = 'true'
    //       AND o.created_at::timestamp >= :start_date
    //       AND o.created_at::timestamp <= :end_date
    //   )
    // `);
      // Add where clause for products if not empty
    if (products && products.length > 0) {
      // Fetch product details to determine which are tiered products
      subqueryForViewsTimeSeries.andWhere("li.variant_id in (select id from product_variant where product_id IN (:...products))", { products });
    }
    if (type) {
      if(type == 'upsell')
      {
        const products = subqueryForTimeSeries.getParameters().variantIds || [];
        let paramIdx = 4;
        const productsPlaceholders = products.length
        ? products.map(() => `$${paramIdx++}`).join(", ")
        : "";
        subqueryForViewsTimeSeries.andWhere(`(
          EXISTS (
            SELECT 1 FROM line_item li2
            WHERE li2.cart_id = c.id
              AND li2.metadata->>'is_upsell' = 'true'
              ${products.length ? `AND li2.variant_id IN (${productsPlaceholders})` : ""}
          )
        )`);
      }
      else if(type == 'services')
      {
        subqueryForViewsTimeSeries.andWhere("li.metadata->>'is_service' = 'true'");
      }
      else if(type == 'courses')
      {
        subqueryForViewsTimeSeries.andWhere("li.metadata->>'is_course' = 'true'");
      }
      else if(type == 'invoices')
      {
        subqueryForViewsTimeSeries.andWhere('c.id in (select cart_id from "draft_order" where invoice_id IS NOT NULL)');
      } 
      else if (type == 'tiered_product') {
        subqueryForViewsTimeSeries.andWhere('c.id in (select cart_id from "order" where purchase_option = :purchase_option AND p.is_tired_product = true)', { purchase_option: 'subscription' } );
      }
      else{
        subqueryForViewsTimeSeries.andWhere('c.id in (select cart_id from "order" where purchase_option = :purchase_option AND p.is_tired_product = false)', { purchase_option: type } );
      }
    }

    subqueryForViewsTimeSeries.andWhere(`
      li.variant_id IN (
        SELECT id FROM product_variant WHERE product_id IN (
          SELECT id FROM product WHERE store_id = :store_id
        )
      )
    `, { store_id: this.store_id });

    subqueryForViewsTimeSeries
      .groupBy("c.metadata->>'pageId', c.id") // Group the results by page ID and cart ID to aggregate view counts
      .orderBy("page_id"); // Order the results by page ID
    // query builder to get new and repeated customers count
    const customerQueryBuilder = this.manager_.createQueryBuilder()
      .select([
        "o.customer_id AS customer_id", // Corrected the alias and the column customer_id was referring from
        "o.created_at AS created_at"
      ])
      .from(LineItem, "li")
      .innerJoin(Order, "o", "o.id = li.order_id")
      .innerJoin(ProductVariant, "pv", "li.variant_id = pv.id")
      .innerJoin(Product, "p", "p.id = pv.product_id") 
      .leftJoin(DraftOrder, "dro", "dro.order_id = o.id")
      .where("o.store_id = :store_id", { store_id: this.store_id })
      .andWhere("o.payment_status IN (:...payment_status)", { payment_status: PAYMENT_STATUS_TO_INCLUDE_IN_REVENUE })

    // refund query builder
    const refundQueryBuilder = this.manager_.createQueryBuilder()
      .select([
        "DISTINCT o.id AS order_id",
        "li.quantity AS quantity",
        "li.unit_price AS unit_price",
      ])
      .from(Refund, 'r')
      .innerJoin(Order, 'o', 'o.id = r.order_id')
      .innerJoin(LineItem, 'li', 'li.order_id = o.id')
      .innerJoin(ProductVariant, 'pv', 'li.variant_id = pv.id')
      .innerJoin(Product, "p", "p.id = pv.product_id") 
      .leftJoin(DraftOrder, "dro", "dro.order_id = o.id")
      .where('o.store_id = :store_id', { store_id: this.store_id })
      .andWhere('o.payment_status IN (:...payment_statuses)', { payment_statuses: [PaymentStatus.PARTIALLY_REFUNDED, PaymentStatus.REFUNDED] })
      .andWhere("r.created_at::timestamp >= :start_date AND r.created_at::timestamp <= :end_date",
        {
          start_date: startDate,
          end_date: queryEndDate(startDate, endDate)  // Using the function to filter the end date as per the input dates
        }
      );

    /**
     * Builds a query to retrieve product view to order conversion counts within a specified date range.
     * This query joins the Cart and Order entities to correlate cart metadata with orders.
     */
    const conversionRateQueryBuilder = this.manager_.createQueryBuilder()
      .select([
        `DATE(o.created_at) AS order_date`, // Selects the date of order creation
        `COALESCE(CAST(COUNT(DISTINCT c.metadata->>'pageId') AS DOUBLE PRECISION), 0) AS conversion_count` // Counts unique pageIds as conversion count, defaults to 0 if none
      ])
      .from(Cart, 'c') // From the Cart entity
      .innerJoin(Order, 'o', `o.page_id = c.metadata->>'pageId'`) // Joins with Order based on pageId
      .innerJoin(LineItem, 'li', 'li.order_id = o.id') // Joins with LineItem based on order id
      .innerJoin(ProductVariant, 'pv', 'li.variant_id = pv.id')
      .innerJoin(Product, "p", "p.id = pv.product_id") 
      .leftJoin(DraftOrder, "dro", "dro.order_id = o.id")
      .where('o.store_id = :store_id', { store_id: this.store_id }) // Filters by the store id of the logged-in user
      .andWhere("o.created_at::timestamp >= :start_date AND o.created_at::timestamp <= :end_date", { // Filters by the date range
        start_date: startDate,
        end_date: endDate
      })
      .groupBy(`DATE(o.created_at)`) // Groups results by order date
      .orderBy(`DATE(o.created_at)`); // Orders results by order date

    /**
     * Constructs a subquery to calculate the conversion rate over time.
     * This query counts distinct pageIds associated with orders, grouped by order creation timestamp.
     */
    const subqueryForConversionRateTimeSeries = this.manager_.createQueryBuilder()
      .select([
      "c.metadata->>'pageId' AS page_id", // Extract the page ID from cart metadata
      "c.id AS cart_id", // Select the cart ID
      "o.created_at AS ordered_on", // Select the creation timestamp of the order
      "COALESCE(CAST(COUNT(DISTINCT c.metadata->>'pageId') AS DOUBLE PRECISION), 0) AS conversion_rate" // Calculate the conversion rate
      ])
      .from(Cart, "c") // Specify the Cart table as the source
      .leftJoin(LineItem, "li", "li.cart_id = c.id")
      .innerJoin(Order, "o", "o.page_id = c.metadata->>'pageId'") // Joins with Order based on pageId
      .innerJoin(ProductVariant, 'pv', 'li.variant_id = pv.id')
      .innerJoin(Product, "p", "p.id = pv.product_id") 
      .leftJoin(DraftOrder, "dro", "dro.order_id = o.id")
      .where("c.created_at::timestamp >= :start_date AND c.created_at::timestamp <= :end_date", {
      start_date: startDate, // Parameter for the start date
      end_date: queryEndDate(startDate, endDate) // Calculate the end date based on the input range
      })
      .andWhere("c.metadata->>'pageId' IS NOT NULL") // Ensure pageId is not null
      .andWhere(new Brackets(qb => {
      qb.where("li.metadata->>'is_refunded' IS NULL").orWhere("li.metadata->>'is_refunded' = 'false'"); // Ensure carts are not refunded
      }))
      .groupBy("c.metadata->>'pageId', c.id, o.created_at"); // Group by necessary fields
    // Add where clause for products if not empty
    if (products && products.length > 0) {
      subqueryForConversionRateTimeSeries.andWhere("li.variant_id in (select id from product_variant where product_id IN (:...products))", { products });
    }
    // Add where clause for type if provided
    if (type) {
      if(type == 'upsell')
      {
        subqueryForConversionRateTimeSeries.andWhere("(EXISTS (SELECT 1 FROM line_item li2 WHERE li2.cart_id = o.cart_id AND li2.metadata->>'is_upsell' = 'true'))");
      }
      else if(type == 'services')
      {
        subqueryForConversionRateTimeSeries.andWhere("li.metadata->>'is_service' = 'true'");
      }
      else if(type == 'courses')
      {
        subqueryForConversionRateTimeSeries.andWhere("li.metadata->>'is_course' = 'true'");
      }
      else if(type == 'invoices')
      {
        subqueryForConversionRateTimeSeries.andWhere('c.id in (select cart_id from "draft_order" where invoice_id IS NOT NULL)');
      } else if (type === 'tiered_product') {
        subqueryForConversionRateTimeSeries.andWhere('c.id in (select cart_id from "order" where purchase_option = :purchase_option AND p.is_tired_product = true)', { purchase_option: 'subscription' } );
      }
      else{
        subqueryForConversionRateTimeSeries.andWhere('c.id in (select cart_id from "order" where purchase_option = :purchase_option AND p.is_tired_product = false)', { purchase_option: type });
      }
    }

    let li2JoinCondition = "li2.order_id = o.id"; // only for ubscription products
    if (type === REPORTS_ORDER_TYPE_ENUM.SUBSCRIPTION) {
      li2JoinCondition += " AND li2.subscription_frequency IS NOT NULL";
    }  else if (type === REPORTS_ORDER_TYPE_ENUM.COURSE) {
      li2JoinCondition += " AND li2.metadata->>'is_course' = 'true'";
    }

    // common query builder
    const queryBuilder = this.manager_.createQueryBuilder()
      // .select([
      // `COALESCE(
      // CAST(
      //   CAST(
      //   SUM(
      //     (li2.unit_price * li2.quantity) - 
      //     CASE
      //       WHEN dr.type IS NOT NULL THEN 
      //         CASE
      //         WHEN dr.type = 'percentage' THEN 
      //           (li.unit_price - COALESCE((gc_total.total_gc_value / line_item_count.count), 0)) * (dr.value / 100) * li.quantity
      //         ELSE 
      //           dr.value / line_item_count.count
      //         END
      //       ELSE 0
      //     END
      //     - COALESCE((gc_total.total_gc_value / line_item_count.count), 0)
      //   ) AS NUMERIC
      //   ) / 100 AS DOUBLE PRECISION
      // ), 0
      // ) AS revenue`,
      // "CAST(COUNT(DISTINCT o.id) AS DOUBLE PRECISION) AS orders"
      // ])
      .select([
        `COALESCE(
          ROUND(
            CAST(
              SUM(
                li2.unit_price * li2.quantity
                - LEAST(COALESCE(gc_total.total_gc_value / line_item_count.count, 0), li2.unit_price * li2.quantity)
                - COALESCE(
                  CASE
                    WHEN dr.type = 'percentage'
                      THEN (li2.unit_price * li2.quantity - LEAST(COALESCE(gc_total.total_gc_value / line_item_count.count, 0), li2.unit_price * li2.quantity)) * (dr.value / 100)
                    WHEN dr.type IS NOT NULL
                      THEN LEAST(dr.value * li2.quantity, li2.unit_price * li2.quantity - LEAST(COALESCE(gc_total.total_gc_value / line_item_count.count, 0), li2.unit_price * li2.quantity))
                    ELSE 0
                  END, 0
                )
              ) AS NUMERIC
            ) / 100, 2
          ), 0
        ) AS revenue`,
        "CAST(COUNT(DISTINCT o.id) AS DOUBLE PRECISION) AS orders"
      ])
      .from(LineItem, "li")
      .innerJoin(Order, "o", "o.id = li.order_id")
      .leftJoin(ProductVariant, "pv", "li.variant_id = pv.id")
      .innerJoin(Product, "p", "p.id = pv.product_id")
      .leftJoin(DraftOrder, "dro", "dro.order_id=o.id")
      .leftJoin("order_discounts", "od", "od.order_id = COALESCE(o.id, dro.order_id)")
      .leftJoin("discount", "d", "od.discount_id = d.id")
      .leftJoin("discount_rule", "dr", "d.rule_id = dr.id")
      .leftJoin(
        qb => qb
          .select("gc.order_id", "order_id")
          .addSelect("SUM(gc.value) AS total_gc_value")
          .from(GiftCard, "gc")
          .groupBy("gc.order_id"),
        "gc_total",
        "gc_total.order_id = dro.order_id"
      )
      .leftJoin(
        qb => qb
          .select("li_count.order_id", "order_id")
          .addSelect("COUNT(*) AS count")
          .from(LineItem, "li_count")
          .groupBy("li_count.order_id"),
        "line_item_count",
        "line_item_count.order_id = dro.order_id"
        )
      // .leftJoin(LineItem, "li2", "li2.order_id = o.id") // Join to include li2 for calculation
      .leftJoin(LineItem, "li2", li2JoinCondition)
      .where("o.store_id = :store_id", { store_id: this.store_id })
      .andWhere("o.payment_status IN (:...payment_status)", { payment_status: PAYMENT_STATUS_TO_INCLUDE_IN_REVENUE })
      .andWhere("o.created_at::timestamp >= :start_date AND o.created_at::timestamp <= :end_date",
      {
        start_date: startDate,
        end_date: queryEndDate(startDate, endDate)  // Using the function to filter the end date as per the input dates
      }
      )

    //#region: AOV calculation starts(filters are not applicable)
   /**
    * get end date with time stamp if selected date todays date else get only date without timestamp
    */
    const [currentAOV, previousAOV, aovTimeSeries] = await Promise.all([
      this.getRevenueAndOrdersCount(queryBuilder, startDate, queryEndDate(startDate, endDate)), // current AOV
      this.getRevenueAndOrdersCount(queryBuilder, previousStartDate, previousEndDate), // previous AOV
      this.getRevenueAndOrdersTimeseries(subqueryForTimeSeries, timeFrame, startDate, queryEndDate(startDate, endDate)), // AOV time-series
    ])
   
    //#endregion

    // Fetch all variant IDs for your selected products
    const variantRows = await this.manager_.query(
      `SELECT id FROM product_variant WHERE product_id = ANY($1::text[])`,
      [products]
    );
    const variantIds = variantRows.map(v => v.id);


    // add `view by product` filter to the various query builders
    // if (products && products.length > 0) {
    //   queryBuilder.andWhere("pv.product_id IN (:...products)", { products })
    //   customerQueryBuilder.andWhere("pv.product_id IN (:...products)", { products })
    //   refundQueryBuilder.andWhere("pv.product_id IN (:...products)", { products })
    //   subqueryForTimeSeries.andWhere("pv.product_id IN (:...products)", { products })
    //   refundLineItemSubquery.andWhere("pv.product_id IN (:...products)", { products })
    // }
    
    // Apply the filter to your query builders
    if (variantIds.length > 0) {
      queryBuilder.andWhere("(li.variant_id IN (:...variantIds) OR li2.variant_id IN (:...variantIds))", { variantIds });
      customerQueryBuilder.andWhere("li.variant_id IN (:...variantIds)", { variantIds });
      refundQueryBuilder.andWhere("li.variant_id IN (:...variantIds)", { variantIds });
      subqueryForTimeSeries.andWhere("li.variant_id IN (:...variantIds)", { variantIds });
      refundLineItemSubquery.andWhere("li.variant_id IN (:...variantIds)", { variantIds });
    }

    function applyProductFilter(products: string[]) {
      queryBuilder.andWhere("(li.variant_id IN (:...variantIds) OR li2.variant_id IN (:...variantIds))", { variantIds });
      customerQueryBuilder.andWhere("li.variant_id IN (:...variantIds)", { variantIds });
      refundQueryBuilder.andWhere("li.variant_id IN (:...variantIds)", { variantIds });
      subqueryForTimeSeries.andWhere("li.variant_id IN (:...variantIds)", { variantIds });
      refundLineItemSubquery.andWhere("li.variant_id IN (:...variantIds)", { variantIds });
    }

    /**
     * Adds a join to the Cart table and applies a filter based on the pageId from the Cart metadata.
     * @param queryToUpdate - The query builder to update with the join and filter.
     * @returns The updated query builder with the Cart join and filter applied.
     */
    function addCartJoinAndFilter(queryToUpdate: any) {
      return queryToUpdate
        .innerJoin(Cart, 'c', 'o.cart_id = c.id') // Join Order to Cart on cart_id
        .andWhere(`o.page_id = c.metadata->>'pageId'`); // Filter results where Order's page_id matches Cart's pageId from metadata
    }


    /**
     * Constructs a query to count the number of distinct carts associated with each page ID
     * within a specified date range. This query aggregates data from the Cart and ProductPage tables.
     *
     * @returns {SelectQueryBuilder<Cart>} - A query builder instance configured to count page views by page ID.
     */
    const viewsQueryBuilder = this.manager_.createQueryBuilder()
      .select([
      "c.metadata->>'pageId' AS page_id", // Extract the page ID from cart metadata
      "CAST(COUNT(DISTINCT c.id) AS NUMERIC) AS view_count", // Count the number of distinct carts with the specified pageId
      ])
      .from(Cart, "c") // Specify the Cart table as the source
      // .leftJoin(ProductPage, "pp", "pp.id = c.metadata->>'pageId'")  // Join with ProductPage based on the extracted page ID
      .leftJoin(LineItem, "li", "li.cart_id = c.id") // Join with LineItem based on cart ID
      .leftJoin(ProductVariant, "pv", "li.variant_id = pv.id") // Join with ProductVariant based on variant ID
      .leftJoin(Product, "p", "p.id = pv.product_id") // Join with Product based on product ID
      .leftJoin(Region, "r", "r.id = c.region_id") // Join with Region based on the region ID
      .where("r.store_id = :store_id", { store_id: this.store_id }) // Ensure the store ID matches
      .andWhere("c.created_at::timestamp >= :start_date AND c.created_at::timestamp <= :end_date", {
        start_date: startDate, // Parameter for the start date
        end_date: queryEndDate(startDate, endDate) // Calculate the end date based on the input range
      })
      .andWhere(new Brackets(qb => {
        qb.where("li.metadata->>'is_refunded' IS NULL").orWhere("li.metadata->>'is_refunded' = 'false'"); // Ensure carts are not refunded
      }))
      .andWhere(`EXISTS (SELECT 1 FROM "order" o WHERE o.cart_id = c.id)`)

    // Add where clause for products if not empty
    if (products && products.length > 0) {
      viewsQueryBuilder.andWhere("li.variant_id in (select id from product_variant where product_id IN (:...products))", { products });
    }

    if(type) {
      if(type == 'upsell')
      {
        const products = subqueryForTimeSeries.getParameters().variantIds || [];
        let paramIdx = 4;
        const productsPlaceholders = products.length
        ? products.map(() => `$${paramIdx++}`).join(", ")
        : "";
        viewsQueryBuilder.andWhere(`(
          EXISTS (
            SELECT 1 FROM line_item li2
            WHERE li2.cart_id = c.id
              AND li2.metadata->>'is_upsell' = 'true'
              ${products.length ? `AND li2.variant_id IN (${productsPlaceholders})` : ""}
          )
        )`);
      }
      else if(type == 'services')
      {
        viewsQueryBuilder.andWhere("li.metadata->>'is_service' = 'true'");
      }
      else if(type == 'courses')
      {
        viewsQueryBuilder.andWhere("li.metadata->>'is_course' = 'true'");
      }
      else if(type == 'invoices')
      {
        viewsQueryBuilder.andWhere('c.id in (select cart_id from "draft_order" where invoice_id IS NOT NULL)');
      } else if (type == 'tiered_product') {
        viewsQueryBuilder.andWhere('c.id in (select cart_id from "order" where purchase_option = :purchase_option AND p.is_tired_product = true)', { purchase_option: 'subscription' } );
      }
      else{
        viewsQueryBuilder.andWhere('c.id in (select cart_id from "order" where purchase_option = :purchase_option AND p.is_tired_product = false)', { purchase_option: type });
      }
    }

    viewsQueryBuilder.andWhere(`
      li.variant_id IN (
        SELECT id FROM product_variant WHERE product_id IN (
          SELECT id FROM product WHERE store_id = :store_id
        )
      )
    `, { store_id: this.store_id });

    viewsQueryBuilder
      .groupBy("c.metadata->>'pageId'") // Group the results by page ID to aggregate view counts
      .orderBy("page_id"); // Order the results by page ID

    // add `type` filter to the various query builders
    switch (type) {
      case REPORTS_ORDER_TYPE_ENUM.ONETIME:
        queryBuilder.andWhere("(o.purchase_option = :purchase_option OR o.purchase_option IS NULL)", { purchase_option: type })
        customerQueryBuilder.andWhere("(o.purchase_option = :purchase_option OR o.purchase_option IS NULL)", { purchase_option: type })
        refundQueryBuilder.andWhere("(o.purchase_option = :purchase_option OR o.purchase_option IS NULL)", { purchase_option: type })
        refundLineItemSubquery.andWhere("(o.purchase_option = :purchase_option OR o.purchase_option IS NULL)", { purchase_option: type })
        subqueryForTimeSeries.andWhere("(o.purchase_option = :purchase_option OR o.purchase_option IS NULL)", { purchase_option: type })
        conversionRateQueryBuilder.andWhere("(o.purchase_option = :purchase_option OR o.purchase_option IS NULL)", { purchase_option: type }) // Filter for orders based on the specified purchase option type (onetime)
        break
      case REPORTS_ORDER_TYPE_ENUM.SUBSCRIPTION:
        queryBuilder.andWhere("o.purchase_option = :purchase_option", { purchase_option: type })
        queryBuilder.andWhere("li.subscription_frequency IS NOT NULL AND (p.is_tired_product = false)")
        customerQueryBuilder.andWhere("o.purchase_option = :purchase_option", { purchase_option: type })
        customerQueryBuilder.andWhere("li.subscription_frequency IS NOT NULL AND (p.is_tired_product = false)")
        refundQueryBuilder.andWhere("o.purchase_option = :purchase_option", { purchase_option: type })
        refundQueryBuilder.andWhere("li.subscription_frequency IS NOT NULL AND (p.is_tired_product = false)")
        refundLineItemSubquery.andWhere("o.purchase_option = :purchase_option", { purchase_option: type })
        refundLineItemSubquery.andWhere("li.subscription_frequency IS NOT NULL AND (p.is_tired_product = false)")
        subqueryForTimeSeries.andWhere("o.purchase_option = :purchase_option", { purchase_option: type })
        subqueryForTimeSeries.andWhere("li.subscription_frequency IS NOT NULL AND (p.is_tired_product = false)")
        conversionRateQueryBuilder.andWhere("o.purchase_option = :purchase_option", { purchase_option: type })
        conversionRateQueryBuilder.andWhere("li.subscription_frequency IS NOT NULL AND (p.is_tired_product = false)"); // Filter for orders based on the specified purchase option type (subscription) and ensure the product is not a tiered product
        break
      case REPORTS_ORDER_TYPE_ENUM.COURSE:
        queryBuilder.andWhere(`li.metadata->> 'is_course' = 'true'`)
        customerQueryBuilder.andWhere(`li.metadata->> 'is_course' = 'true'`)
        refundQueryBuilder.andWhere(`li.metadata->> 'is_course' = 'true'`)
        refundLineItemSubquery.andWhere(`li.metadata->> 'is_course' = 'true'`)
        subqueryForTimeSeries.andWhere(`li.metadata->> 'is_course' = 'true'`)
        conversionRateQueryBuilder.andWhere(`li.metadata->> 'is_course' = 'true'`); // Filter for line items that are marked as courses
        break
      case REPORTS_ORDER_TYPE_ENUM.SERVICE:
        queryBuilder.andWhere(`li.metadata->> 'is_service' = 'true'`)  // Add a condition to the main query to filter line items where the metadata indicates they are services
        customerQueryBuilder.andWhere(`li.metadata->> 'is_service' = 'true'`)   // Add the same condition to the customer query to filter customers based on service items
        refundQueryBuilder.andWhere(`li.metadata->> 'is_service' = 'true'`)    // Add the same condition to the refund query to filter refunds based on service items
        refundLineItemSubquery.andWhere(`li.metadata->> 'is_service' = 'true'`) // Add the same condition to the refund line item subquery to filter refunds based on service items
        subqueryForTimeSeries.andWhere(`li.metadata->> 'is_service' = 'true'`)   // Add the same condition to the subquery for time series to filter based on service items
        break
      case REPORTS_ORDER_TYPE_ENUM.INVOICE:
        // instead of metadata we store invoice flag in invoice_id column in draft_order table
        queryBuilder.andWhere(`dro.invoice_id IS NOT NULL`)  // Add a condition to the main query to filter line items where the metadata indicates they are services
        customerQueryBuilder.andWhere(`dro.invoice_id IS NOT NULL`)   // Add the same condition to the customer query to filter customers based on service items
        refundQueryBuilder.andWhere(`dro.invoice_id IS NOT NULL`)    // Add the same condition to the refund query to filter refunds based on service items
        refundLineItemSubquery.andWhere(`dro.invoice_id IS NOT NULL`) // Add the same condition to the refund line item subquery to filter refunds based on service items
        subqueryForTimeSeries.andWhere(`dro.invoice_id IS NOT NULL`)   // Add the same condition to the subquery for time series to filter based on service items
        break
      case REPORTS_ORDER_TYPE_ENUM.UPSELL:
        queryBuilder.andWhere(`li.metadata->> 'is_upsell' = 'true'`)
        customerQueryBuilder.andWhere(`li.metadata->> 'is_upsell' = 'true'`)
        refundQueryBuilder.andWhere(`li.metadata->> 'is_upsell' = 'true'`)
        refundLineItemSubquery.andWhere(`li.metadata->> 'is_upsell' = 'true'`)
        subqueryForTimeSeries.andWhere(`li.metadata->> 'is_upsell' = 'true'`)
        conversionRateQueryBuilder.andWhere(`li.metadata->> 'is_upsell' = 'true'`); // Filter for line items that are marked as upsells
        break
      case REPORTS_ORDER_TYPE_ENUM.PAGEVIEWS: // Data to be filtered as per the report page views
        queryBuilder.andWhere(`li.metadata = '{}'::jsonb`)  // Filter line items to only include those with empty metadata
        customerQueryBuilder.andWhere(`li.metadata = '{}'::jsonb`)  // Apply the same filter for the customer query builder
        refundQueryBuilder.andWhere(`li.metadata = '{}'::jsonb`)  // Apply the same filter for the refund query builder
        refundLineItemSubquery.andWhere(`li.metadata = '{}'::jsonb`)  // Apply the same filter for the refund line item subquery
        subqueryForViewsTimeSeries.andWhere("c.metadata IS NOT NULL AND c.metadata != '{}'::jsonb")  // Ensure the metadata in the Cart is not null and is not an empty JSON object
        break
      case REPORTS_ORDER_TYPE_ENUM.CONVERSIONRATE:   // Handle the case for REPORTS_ORDER_TYPE_ENUM.CONVERSIONRATE
        addCartJoinAndFilter(queryBuilder);          // Add Cart join and filter to the main query builder
        addCartJoinAndFilter(customerQueryBuilder);  // Add Cart join and filter to the customer-specific query builder
        addCartJoinAndFilter(refundQueryBuilder);    // Add Cart join and filter to the refund query builder
        addCartJoinAndFilter(refundLineItemSubquery); // Add Cart join and filter to the refund line item subquery
        addCartJoinAndFilter(subqueryForTimeSeries); // Add Cart join and filter to the time series subquery
        break;
      case REPORTS_ORDER_TYPE_ENUM.VIRTUALTERMINAL:
        queryBuilder.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true'`)  // Add a condition to the main query to filter line items where the metadata indicates they are services
        customerQueryBuilder.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true'`)   // Add the same condition to the customer query to filter customers based on service items
        refundQueryBuilder.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true'`)    // Add the same condition to the refund query to filter refunds based on service items
        refundLineItemSubquery.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true'`)   // Add the same condition to the refund line item subquery to filter refunds based on service items
        subqueryForTimeSeries.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true'`)   // Add the same condition to the subquery for time series to filter based on service items
        if (products && products.length > 0) applyProductFilter(products);
        break
      case REPORTS_ORDER_TYPE_ENUM.VIRTUALTERMINALCARD:
        queryBuilder.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'card' OR o.metadata->> 'transactionType' = 'card')`)
        customerQueryBuilder.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'card' OR o.metadata->> 'transactionType' = 'card')`)
        refundQueryBuilder.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'card' OR o.metadata->> 'transactionType' = 'card')`)
        refundLineItemSubquery.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'card' OR o.metadata->> 'transactionType' = 'card')`)
        subqueryForTimeSeries.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'card' OR o.metadata->> 'transactionType' = 'card')`)
        if (products && products.length > 0) applyProductFilter(products);
        break
      case REPORTS_ORDER_TYPE_ENUM.VIRTUALTERMINALCASH:
        queryBuilder.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'cash' OR o.metadata->> 'transactionType' = 'cash')`)
        customerQueryBuilder.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'cash' OR o.metadata->> 'transactionType' = 'cash')`)
        refundQueryBuilder.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'cash' OR o.metadata->> 'transactionType' = 'cash')`)
        refundLineItemSubquery.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'cash' OR o.metadata->> 'transactionType' = 'cash')`)
        subqueryForTimeSeries.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'cash' OR o.metadata->> 'transactionType' = 'cash')`)
        if (products && products.length > 0) applyProductFilter(products);
        break
      case REPORTS_ORDER_TYPE_ENUM.VIRTUALTERMINALCLOVER:
        queryBuilder.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'clover' OR o.metadata->> 'transactionType' = 'clover')`)
        customerQueryBuilder.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'clover' OR o.metadata->> 'transactionType' = 'clover')`)
        refundQueryBuilder.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'clover' OR o.metadata->> 'transactionType' = 'clover')`)
        refundLineItemSubquery.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'clover' OR o.metadata->> 'transactionType' = 'clover')`)
        subqueryForTimeSeries.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'clover' OR o.metadata->> 'transactionType' = 'clover')`)
        if (products && products.length > 0) applyProductFilter(products);
        break
      case REPORTS_ORDER_TYPE_ENUM.VIRTUALTERMINALACH:
        queryBuilder.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'ach' OR o.metadata->> 'transactionType' = 'ach')`)
        customerQueryBuilder.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'ach' OR o.metadata->> 'transactionType' = 'ach')`)
        refundQueryBuilder.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'ach' OR o.metadata->> 'transactionType' = 'ach')`)
        refundLineItemSubquery.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'ach' OR o.metadata->> 'transactionType' = 'ach')`)
        subqueryForTimeSeries.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'ach' OR o.metadata->> 'transactionType' = 'ach')`)
        if (products && products.length > 0) applyProductFilter(products);
        break
      case REPORTS_ORDER_TYPE_ENUM.TIEREDPRODUCT:
        queryBuilder.andWhere("(p.is_tired_product = true)");
        customerQueryBuilder.andWhere("(p.is_tired_product = true)", { purchase_option: type });
        refundQueryBuilder.andWhere("(p.is_tired_product = true)");
        refundLineItemSubquery.andWhere("(p.is_tired_product = true)");
        subqueryForTimeSeries.andWhere("(p.is_tired_product = true)");
        break;
      default:
        break
    }

    //#region: Revenue, Orders, Cutomers and Refund calculation starts (with filters)
  /**
    * get end date with time stamp if selected date todays date else get only date without timestamp
    */
    const [currrentRevenueAndOrders, previousRevenueAndOrders, revenueAndOrdersTimeseries, previousRevenueAndOrdersTimeseries, currentNewCustomers, previousNewCustomers, currentRepeatedCustomers, previousRepeatedCustomers] = await Promise.all([
      this.getRevenueAndOrdersCount(queryBuilder, startDate, queryEndDate(startDate, endDate)), // current revenue and orders
      this.getRevenueAndOrdersCount(queryBuilder, previousStartDate, previousEndDate), // previous revenue and orders
      this.getRevenueAndOrdersTimeseries(subqueryForTimeSeries, timeFrame, startDate, queryEndDate(startDate, endDate)), // revenue and orders time-series
      this.getRevenueAndOrdersTimeseries(subqueryForTimeSeries, timeFrame, previousStartDate, previousEndDate),
      this.getCustomersCount(customerQueryBuilder, startDate, queryEndDate(startDate, endDate), true), // current new customers
      this.getCustomersCount(customerQueryBuilder, previousStartDate, previousEndDate, true), // previous new customers
      this.getCustomersCount(customerQueryBuilder, startDate, queryEndDate(startDate, endDate), false), // current repeated customers
      this.getCustomersCount(customerQueryBuilder, previousStartDate, previousEndDate, false) // previous repeated customers
    ])

      const [currentVoid, previousVoid] = await Promise.all([
        this.getVoidRevenueTimeseries(voidQueryBuilder, timeFrame, startDate, queryEndDate(startDate, endDate)), // real-time voids
        this.getVoidRevenueTimeseries(voidQueryBuilder, timeFrame, previousStartDate, previousEndDate) // previous voids
      ])

    console.log('currentVoid', currentVoid);
    console.log('previousVoid', previousVoid);

    // update select fields in the common subqueryForTimeSeries for the customer count calculation
    subqueryForTimeSeries.select([
      "o.id AS order_id",
      "o.created_at AS created_at",
      "o.customer_id AS customer_id"
    ])
   /**
    * get end date with time stamp if selected date todays date else get only date without timestamp 
    */
    const [repeatedCustomersTimeSeries, newCustomersTimeSeries, previousRepeatedCustomersTimeSeries, previousNewCustomersTimeSeries] = await Promise.all([
      this.getRepeatCustomersTimeSeries(subqueryForTimeSeries, timeFrame, startDate, queryEndDate(startDate, endDate), type), // repeated customers time-series
      this.getRepeatCustomersTimeSeries(subqueryForTimeSeries, timeFrame, startDate, queryEndDate(startDate, endDate), type, true), // repeated customers time-series
      // this.getCustomersTimeSeries(subqueryForTimeSeries, timeFrame, startDate, queryEndDate(startDate, endDate), true), // new customers time-series
      this.getRepeatCustomersTimeSeries(subqueryForTimeSeries, timeFrame, previousStartDate, previousEndDate, type), // previous repeated customers time-series
      this.getRepeatCustomersTimeSeries(subqueryForTimeSeries, timeFrame, previousStartDate, previousEndDate, type, true) // previous new customers time-series
    ])

    // refund calculation

    // update select fields and add Refund table as inner join in the common subqueryForTimeSeries for the refund amount calculation
    subqueryForTimeSeries
      .select([
        "DISTINCT o.id AS order_id",
        "li.quantity AS quantity",
        "li.unit_price AS unit_price",
        "MAX(r.created_at) AS created_at"
      ])
      .innerJoin(Refund, "r", "r.order_id = o.id")
      .groupBy("o.id")
      .addGroupBy("li.quantity")
      .addGroupBy("li.unit_price")

    const [currentRefunds, previousRefunds, refundsTimeSeries, previousRefundsTimeSeries] = await Promise.all([
      this.getRefundAmount(refundQueryBuilder, startDate, endDate), // real-time refunds)
      this.getRefundAmount(refundQueryBuilder, previousStartDate, previousEndDate), // previous refunds
      this.getRefundTimeseries(refundLineItemSubquery, timeFrame, startDate,endDate), // refunds time-series
      this.getRefundTimeseries(previousRefundLineItemSubquery, timeFrame, previousStartDate, previousEndDate)
    ])

    // Execute multiple asynchronous operations in parallel using to fetch data for the Views Report
    const [currentViews, previousViews, pageViewsTimeSeries] = await Promise.all([
      this.getTotalViewsCount(viewsQueryBuilder, startDate, queryEndDate(startDate, queryEndDate(startDate, endDate))), // Fetch the total views count for the current date range
      this.getTotalViewsCount(viewsQueryBuilder, previousStartDate, previousEndDate),  // Fetch the total views count for the previous date range
      this.getTotalViewsTimeseries(subqueryForViewsTimeSeries, timeFrame, startDate, queryEndDate(startDate, endDate)) // Fetch the time-series data for page views over the specified time frame
    ]);

    // Execute multiple asynchronous operations in parallel using Promise.all
    const [currentConversionRate, previousConversionRate, conversionRateTimeSeries] = await Promise.all([
      this.getConversionRateCount(conversionRateQueryBuilder, startDate, queryEndDate(startDate, endDate)), // Get the current conversion rate count for the specified date rang
      this.getConversionRateCount(conversionRateQueryBuilder, previousStartDate, previousEndDate), // Get the previous conversion rate count for the previous date rang
      this.getConversionRateTimeseries(subqueryForConversionRateTimeSeries, timeFrame, startDate, queryEndDate(startDate, endDate)) // Get the conversion rate time series data for the specified timeframe and date range
    ]);

    //#endregion

    // final response
    const reports: StoreOrderStats = {
      revenue: {
        current_value: revenueAndOrdersTimeseries.reduce((sum, tsd) => sum + Number(tsd.revenue), 0),
        previous_value: previousRevenueAndOrdersTimeseries.reduce((sum, tsd) => sum + Number(tsd.revenue), 0),
        time_series: revenueAndOrdersTimeseries.map(tsd => ({
          start: format(tsd.start_date, DATE_FORMATS.iso8601),
          end: format(subMilliseconds(tsd.end_date, 1), DATE_FORMATS.iso8601EOD),
          value: tsd.revenue
        }))
      },
      orders: {
        current_value: revenueAndOrdersTimeseries.reduce((sum, tsd) => sum + Number(tsd.orders), 0),
        previous_value: previousRevenueAndOrdersTimeseries.reduce((sum, tsd) => sum + Number(tsd.orders), 0),
        time_series: revenueAndOrdersTimeseries.map(tsd => ({
          start: format(tsd.start_date, DATE_FORMATS.iso8601),
          end: format(subMilliseconds(tsd.end_date, 1), DATE_FORMATS.iso8601EOD),
          value: tsd.orders
        }))
      },
      void:{
        current_value: currentVoid.reduce((sum, tsd) => sum + Number(tsd.voided_revenue), 0),
        previous_value: previousVoid.reduce((sum, tsd) => sum + Number(tsd.voided_revenue), 0),
        time_series: []
      },
      refunds: {
        current_value: refundsTimeSeries.reduce((sum, tsd) => sum + Number(tsd.refunds), 0),
        previous_value: previousRefundsTimeSeries ? previousRefundsTimeSeries.reduce((sum, tsd) => sum + Number(tsd.refunds), 0) : 0,
        time_series: refundsTimeSeries.map(tsd => ({
          start: format(tsd.start_date, DATE_FORMATS.iso8601),
          end: format(subMilliseconds(tsd.end_date, 1), DATE_FORMATS.iso8601EOD),
          value: Number(tsd.refunds)
        }))
      },
      aov: {
        // current_value: (currentAOV.revenue / (currentAOV.orders)) || 0,
        current_value: (revenueAndOrdersTimeseries.reduce((sum, tsd) => sum + Number(tsd.revenue), 0) / (revenueAndOrdersTimeseries.reduce((sum, tsd) => sum + Number(tsd.orders), 0))) || 0,
        previous_value: (previousRevenueAndOrdersTimeseries.reduce((sum, tsd) => sum + Number(tsd.revenue), 0) / previousRevenueAndOrdersTimeseries.reduce((sum, tsd) => sum + Number(tsd.orders), 0)) || 0,
        time_series: revenueAndOrdersTimeseries.map(tsd => ({
          start: format(tsd.start_date, DATE_FORMATS.iso8601),
          end: format(subMilliseconds(tsd.end_date, 1), DATE_FORMATS.iso8601EOD),
          value: (tsd.revenue / tsd.orders) || 0
        }))
            },
            repeated_customers: {
        current_value: Array.from(
          new Set(
            repeatedCustomersTimeSeries.flatMap(tsd => tsd.customer_ids || [])
          )
        ).length,
        previous_value: Array.from(
          new Set(
            previousRepeatedCustomersTimeSeries.flatMap(tsd => tsd.customer_ids || [])
          )
        ).length,
        time_series: repeatedCustomersTimeSeries.map(tsd => ({
          start: format(tsd.start_date, DATE_FORMATS.iso8601),
          end: format(subMilliseconds(tsd.end_date, 1), DATE_FORMATS.iso8601EOD),
          value: Array.from(new Set(tsd.customer_ids || [])).length
        }))
            },
            new_customers: {
        current_value: Array.from(
          new Set(
            newCustomersTimeSeries.flatMap(tsd => tsd.customer_ids || [])
          )
        ).length,
        previous_value: Array.from(
          new Set(
            previousNewCustomersTimeSeries.flatMap(tsd => tsd.customer_ids || [])
          )
        ).length,
        time_series: newCustomersTimeSeries.map(tsd => ({
          start: format(tsd.start_date, DATE_FORMATS.iso8601),
          end: format(subMilliseconds(tsd.end_date, 1), DATE_FORMATS.iso8601EOD),
          value: Array.from(new Set(tsd.customer_ids || [])).length
        }))
            },
            conversion_rate: {  // Conversion Rate Order Stats
        // current_value: currentConversionRate.conversions, // Current conversion count
        current_value: ((revenueAndOrdersTimeseries.reduce((sum, tsd) => sum + Number(tsd.orders), 0) / pageViewsTimeSeries.reduce((sum, tsd) => sum + Number(tsd.views), 0)) * 100) || 0, // Current conversion count
        previous_value: previousConversionRate.conversions, // Previous conversion count
        time_series: conversionRateTimeSeries.map(tsd => ({ // Map the time series data
          start: format(tsd.start_date, DATE_FORMATS.iso8601), // Format start date to ISO 8601
          end: format(subMilliseconds(tsd.end_date, 1), DATE_FORMATS.iso8601EOD), // Format end date to ISO 8601 EOD
          value: tsd.conversions // Conversion count for the time series entry
        }))
      },
      // Append Page Views data to the Order Statistics data
      page_views: {
        current_value: pageViewsTimeSeries.reduce((sum, tsd) => sum + Number(tsd.views), 0), // Convert the current views count to a number
        previous_value: Number(previousViews.views), // Convert the previous views count to a number
        time_series: pageViewsTimeSeries.map(tsd => ({ // Map over the time series data to format it for display
          start: format(tsd.start_date, DATE_FORMATS.iso8601), // Format the start date to ISO 8601 format
          end: format(tsd.end_date, DATE_FORMATS.iso8601EOD), // Format the end date to ISO 8601 End of Day format
          value: Number(tsd.views) // Convert the views count for each time series data point to a number
        }))
      },

    }

    return reports
  }

  /**
   * @description Get time-series subquery
   * @param timeFrame "hour" | "day" | "week" | "month"
   * @returns string
   */
  protected getTimeSeriesSubquery(timeFrame: TimeFrame): string {
    // date range subquery
    const dateRange = this.manager_.createQueryBuilder()
      .select(`generate_series(:start_date::timestamp, :end_date::timestamp, interval '1 ${timeFrame}')`, 'start_date')
      .addSelect(`generate_series(:start_date::timestamp + interval '1 ${timeFrame}', :end_date::timestamp, interval '1 ${timeFrame}')`, 'end_date')
      .from('(select 1)', 'series')

    // date series subquery
    return this.manager_.createQueryBuilder()
      .select('start_date', 'start_date')
      .addSelect('COALESCE(end_date, :end_date::timestamp)', 'end_date')
      .from("(" + dateRange.getQuery() + ")", "date_range")
      .getQuery()
  }

  /**
   * @description Get revenue and orders count
   * @param queryBuilder SelectQueryBuilder<LineItem>
   * @param startDate string
   * @param endDate string
   * @returns Promise<{ revenue: number, orders: number }>
   */
  protected async getRevenueAndOrdersCount(queryBuilder: SelectQueryBuilder<LineItem>, startDate: string, endDate: string): Promise<{ revenue: number, orders: number }> {

    const response = await queryBuilder
    .setParameters({ start_date: startDate, end_date: endDate })
    .groupBy("p.id")
    .addGroupBy("li.unit_price")
    .getRawMany()

    let revenue = 0;
    let orders = 0;
    response.forEach(value => {
      revenue += value.revenue
      orders += value.orders
    });

    return { revenue: revenue, orders: orders }
  }

  /**
   * @description Get revenue and orders timeseries
   * @param subqueryForTimeSeries SelectQueryBuilder<LineItem>
   * @param timeFrame "hour" | "day" | "week" | "month"
   * @param startDate string
   * @param endDate string
   * @returns 
   */
  protected async getRevenueAndOrdersTimeseries(subqueryForTimeSeries: SelectQueryBuilder<LineItem>, timeFrame: TimeFrame, startDate: string, endDate: string): Promise<{ start_date: Date, end_date: Date, revenue: number, orders: number }[]> {
    const params = { // Separated parameters to ease understanding and modifications
      start_date: startDate,
      end_date: queryEndDate(startDate, endDate),  // Using the function to filter the end date as per the input dates
      payment_status: PAYMENT_STATUS_TO_INCLUDE_IN_REVENUE,
      store_id: this.store_id,
      ...subqueryForTimeSeries.getParameters()
    };

    return await this.manager_.createQueryBuilder()
      // .select([
      //   "start_date",
      //   "end_date",
      //   `COALESCE(
      //     CAST(
      //       CAST(
      //         SUM(
      //           CAST(quantity AS NUMERIC) *
      //           (
      //             CASE
      //               WHEN payment_status = '${PaymentStatus.PARTIALLY_REFUNDED}'
      //                 THEN
      //                   CASE
      //                     WHEN (sub.metadata->>'refunded_amount') IS NOT NULL
      //                       THEN CAST(unit_price AS NUMERIC) - CAST(sub.metadata->>'refunded_amount' AS NUMERIC)
      //                     ELSE CAST(unit_price AS NUMERIC)
      //                   END
      //               ELSE CAST(unit_price AS NUMERIC)
      //             END
      //             -- Deduct gift card and discount
      //             - COALESCE(
      //               CASE
      //                 WHEN dr.type = 'percentage' THEN unit_price * (dr.value / 100) * quantity
      //                 ELSE LEAST(dr.value * quantity, unit_price * quantity)
      //               END, 0
      //             )
      //             - LEAST(COALESCE(gc_total.total_gc_value / line_item_count.count, 0), sub.unit_price * sub.quantity)
      //           )
      //         ) AS NUMERIC
      //       ) / 100 AS DOUBLE PRECISION
      //     ), 0
      //   ) AS revenue`,
      //   "CAST(COUNT(DISTINCT sub.order_id) AS DOUBLE PRECISION) AS orders"
      // ])
      .select([
        "start_date",
        "end_date",
        `COALESCE(
          CAST(
            CAST(
              SUM(
                GREATEST(
                  CAST(quantity AS NUMERIC) *
                  (
                    CASE
                      WHEN payment_status = '${PaymentStatus.PARTIALLY_REFUNDED}'
                        THEN
                          CASE
                            WHEN (sub.metadata->>'refunded_amount') IS NOT NULL
                              THEN CAST(unit_price AS NUMERIC) - CAST(sub.metadata->>'refunded_amount' AS NUMERIC)
                            ELSE CAST(unit_price AS NUMERIC)
                          END
                      ELSE CAST(unit_price AS NUMERIC)
                    END
                    - LEAST(COALESCE(gc_total.total_gc_value / line_item_count.count, 0), sub.unit_price * sub.quantity)
                    - COALESCE(
                      CASE
                        WHEN dr.type = 'percentage'
                          THEN (sub.unit_price * sub.quantity - LEAST(COALESCE(gc_total.total_gc_value / line_item_count.count, 0), sub.unit_price * sub.quantity)) * (dr.value / 100)
                        WHEN dr.type IS NOT NULL
                          THEN LEAST(dr.value * sub.quantity, sub.unit_price * sub.quantity - LEAST(COALESCE(gc_total.total_gc_value / line_item_count.count, 0), sub.unit_price * sub.quantity))
                        ELSE 0
                      END, 0
                    )
                  ), 0
                )
              ) AS NUMERIC
            ) / 100 AS DOUBLE PRECISION
          ), 0
        ) AS revenue`,
        "CAST(COUNT(DISTINCT sub.order_id) AS DOUBLE PRECISION) AS orders"
      ])
      .from("(" + this.getTimeSeriesSubquery(timeFrame) + ")", "ds")
      .leftJoin("(" + subqueryForTimeSeries.getQuery() + ")", "sub", "created_at::timestamp >= start_date AND created_at::timestamp < end_date")
      // Join gift card total per order from gift_card_transaction
      .leftJoin(
        qb => qb
          .select("gct.order_id", "order_id")
          .addSelect("SUM(gct.amount) AS total_gc_value")
          .from("gift_card_transaction", "gct")
          .groupBy("gct.order_id"),
        "gc_total",
        "gc_total.order_id = sub.order_id"
      )
      // Join discount total per order
      .leftJoin(
        qb => qb
          .select("li_disc.order_id", "order_id")
          .addSelect("SUM(CASE WHEN dr.type = 'percentage' THEN li_disc.unit_price * (dr.value / 100) * li_disc.quantity ELSE dr.value * li_disc.quantity END) AS total_discount")
          .from(LineItem, "li_disc")
          .innerJoin("order_discounts", "od", "od.order_id = li_disc.order_id")
          .innerJoin("discount", "d", "od.discount_id = d.id")
          .innerJoin("discount_rule", "dr", "d.rule_id = dr.id")
          .groupBy("li_disc.order_id"),
        "discount_total",
        "discount_total.order_id = sub.order_id"
      )
      .leftJoin("order_discounts", "od", "od.order_id = sub.order_id")
      .leftJoin("discount", "d", "od.discount_id = d.id")
      .leftJoin("discount_rule", "dr", "d.rule_id = dr.id")
      // Join line item count per order
      .leftJoin(
        qb => qb
          .select("li_count.order_id", "order_id")
          .addSelect("COUNT(*) AS count")
          .from(LineItem, "li_count")
          .groupBy("li_count.order_id"),
        "line_item_count",
        "line_item_count.order_id = sub.order_id"
      )
      .addGroupBy("start_date")
      .addGroupBy("end_date")
      .addOrderBy("start_date")
      .setParameters(params)
      .getRawMany()
  }

  protected async getVoidRevenueTimeseries(
    subqueryForTimeSeries: SelectQueryBuilder<LineItem>,
    timeFrame: TimeFrame,
    startDate: string,
    endDate: string
  ): Promise<{ start_date: Date, end_date: Date, voided_revenue: number }[]> {

    const params = {
      start_date: startDate,
      end_date: queryEndDate(startDate, endDate),
      payment_status: [PaymentStatus.VOIDED],
      store_id: this.store_id,
      ...subqueryForTimeSeries.getParameters()
    }

    return await this.manager_.createQueryBuilder()
      .select([
        "start_date",
        "end_date",
        `COALESCE(
          CAST(
            CAST(
              SUM(
                CAST(quantity AS NUMERIC) *
                (
                  CAST(unit_price AS NUMERIC)
                  - LEAST(COALESCE(gc_total.total_gc_value / line_item_count.count, 0), sub.unit_price * sub.quantity)
                  - COALESCE(
                    CASE
                      WHEN dr.type = 'percentage'
                        THEN (sub.unit_price * sub.quantity - LEAST(COALESCE(gc_total.total_gc_value / line_item_count.count, 0), sub.unit_price * sub.quantity)) * (dr.value / 100)
                      WHEN dr.type IS NOT NULL
                        THEN LEAST(dr.value * sub.quantity, sub.unit_price * sub.quantity - LEAST(COALESCE(gc_total.total_gc_value / line_item_count.count, 0), sub.unit_price * sub.quantity))
                      ELSE 0
                    END, 0
                  )
                )
              ) AS NUMERIC
            ) / 100 AS DOUBLE PRECISION
          ), 0
        ) AS voided_revenue`
      ])
      .from("(" + this.getTimeSeriesSubquery(timeFrame) + ")", "ds")
      .leftJoin("(" + subqueryForTimeSeries.getQuery() + ")", "sub",
        "sub.updated_at::timestamp >= start_date AND sub.updated_at::timestamp < end_date AND sub.payment_status = :void_status"
      )

      .leftJoin(
        qb => qb
          .select("gct.order_id", "order_id")
          .addSelect("SUM(gct.amount) AS total_gc_value")
          .from("gift_card_transaction", "gct")
          .groupBy("gct.order_id"),
        "gc_total",
        "gc_total.order_id = sub.order_id"
      )
      .leftJoin("order_discounts", "od", "od.order_id = sub.order_id")
      .leftJoin("discount", "d", "od.discount_id = d.id")
      .leftJoin("discount_rule", "dr", "d.rule_id = dr.id")
      .leftJoin(
        qb => qb
          .select("li_count.order_id", "order_id")
          .addSelect("COUNT(*) AS count")
          .from(LineItem, "li_count")
          .groupBy("li_count.order_id"),
        "line_item_count",
        "line_item_count.order_id = sub.order_id"
      )
      .addGroupBy("start_date")
      .addGroupBy("end_date")
      .addOrderBy("start_date")
      .setParameters({
        ...params,
        void_status: PaymentStatus.VOIDED
      })
      .getRawMany();
  }


  /**
   * @description Get refund amount
   * @param refundQueryBuilder SelectQueryBuilder<Refund>
   * @param startDate string
   * @param endDate string
   * @returns Promise<{ refunds: number }>
   */
  protected async getRefundAmount(refundQueryBuilder: SelectQueryBuilder<Refund>, startDate: string, endDate: string): Promise<{ refunds: number }> {
    return await this.manager_.createQueryBuilder()
      .select("COALESCE(CAST(CAST(SUM(CAST(quantity AS NUMERIC) * CAST(unit_price AS NUMERIC)) AS NUMERIC) / 100 AS DOUBLE PRECISION), 0) AS refunds")
      .from("(" + refundQueryBuilder.getQuery() + ")", "sub")
      .setParameters({ start_date: startDate, end_date: endDate, ...refundQueryBuilder.getParameters() })
      .getRawOne()
  }

  /**
   * Retrieves the total views count for a specified date range.
   * This function executes a subquery to aggregate view counts and returns the result.
   *
   * @param {SelectQueryBuilder<Cart>} viewsQueryBuilder - The query builder instance configured for counting views.
   * @param {string} startDate - The start date for the views count.
   * @param {string} endDate - The end date for the views count.
   * @returns {Promise<{ views: number }>} - A promise that resolves to an object containing the views count.
   */
  protected async getTotalViewsCount(
    viewsQueryBuilder: SelectQueryBuilder<Cart>,
    startDate: string,
    endDate: string
  ): Promise<{ views: number }> {
    return await this.manager_.createQueryBuilder()
      .select("COALESCE(CAST(SUM(view_count) AS NUMERIC), 0) AS views")  // Sum the view_count and handle null with COALESCE
      .from(`(${viewsQueryBuilder.getQuery()})`, "sub")  // Use the viewsQueryBuilder's query as a subquery
      .setParameters({  // Set the parameters required i nthe query to retrieve the appropriate data for page views count
        start_date: startDate, // Set the start date parameter for the query
        end_date: endDate,     // Set the end date parameter for the query
        ...viewsQueryBuilder.getParameters() // Include any additional parameters from the original query builder
      })
      .getRawOne(); // Retrieve a single row containing the views count
  }

  /**
   * @description Get refund timeseries
   * @param subqueryForTimeSeries SelectQueryBuilder<LineItem>
   * @param timeFrame "hour" | "day" | "week" | "month"
   * @param startDate string
   * @param endDate string
   * @returns 
   */
  protected async getRefundTimeseries(subqueryForTimeSeries: SelectQueryBuilder<LineItem>, timeFrame: TimeFrame, startDate: string, endDate: string): Promise<{ start_date: Date, end_date: Date, refunds: number }[]> {
    const params = { // Separated parameters to ease understanding and modifications
      start_date: startDate,
      end_date: queryEndDate(startDate, endDate),  // Using the function to filter the end date as per the input dates
      payment_status: [PaymentStatus.REFUNDED, PaymentStatus.PARTIALLY_REFUNDED],
      store_id: this.store_id,
      ...subqueryForTimeSeries.getParameters()
    };

    return await this.manager_.createQueryBuilder()
      .select([
      "start_date",
      "end_date",
      "COALESCE(SUM(sub.refund_amount) / 100, 0) AS refunds",
      ])
      .from("(" + this.getTimeSeriesSubquery(timeFrame) + ")", "ds")
      .leftJoin("(" + subqueryForTimeSeries.getQuery() + ")", "sub", "created_at::timestamp >= start_date AND created_at::timestamp < end_date")
      .addGroupBy("start_date")
      .addGroupBy("end_date")
      .addOrderBy("start_date")
      .setParameters(params)
      .getRawMany();
  }


  /**
   * Retrieves the total conversion count based on the provided query builder and date range.
   * @param conversionRateQueryBuilder - The query builder for conversion rate data.
   * @param startingDate - The start date for the conversion count.
   * @param endingDate - The end date for the conversion count.
   * @returns A promise that resolves to an object containing the total conversions.
   */
  protected async getConversionRateCount(
    conversionRateQueryBuilder: SelectQueryBuilder<Cart>,
    startingDate: string,
    endingDate: string
  ): Promise<{ conversions: number }> {
    return await this.manager_.createQueryBuilder()
      .select(`COALESCE(CAST(SUM(conversion_count) AS DOUBLE PRECISION), 0) AS conversions`) // Sum conversion counts, default to 0 if none
      .from(`(${conversionRateQueryBuilder.getQuery()})`, "sub") // Use the provided query builder as a subquery
      .setParameters({ // Set parameters for the query
        start_date: startingDate, // Start date for filtering
        end_date: endingDate, // End date for filtering
        ...conversionRateQueryBuilder.getParameters() // Include any additional parameters from the query builder
      })
      .getRawOne(); // Execute the query and return a single raw result
  }

  /**
 * Retrieves the conversion rate time series data based on the provided subquery and date range.
 * @param subqueryForTimeSeries - The query builder for the time series data.
 * @param timeFrame - The time frame for the time series (e.g., daily, weekly).
 * @param startingDate - The start date for the time series.
 * @param endingDate - The end date for the time series.
 * @returns A promise that resolves to an array of objects containing start and end dates along with conversion counts.
 */
  protected async getConversionRateTimeseries(
    subqueryForTimeSeries: SelectQueryBuilder<Cart>,
    timeFrame: TimeFrame,
    startingDate: string,
    endingDate: string
  ): Promise<{ start_date: Date; end_date: Date; conversions: number }[]> {
    return await this.manager_.createQueryBuilder()
      .select([
        "start_date", // Select the start date
        "end_date", // Select the end date
        "COALESCE(CAST(COUNT(conversion_rate) AS DOUBLE PRECISION), 0) AS conversions" // Count conversion rates, default to 0 if none
      ])
      .from(`(${this.getTimeSeriesSubquery(timeFrame)})`, "ds") // Use the time series subquery
      .leftJoin(`(${subqueryForTimeSeries.getQuery()})`, "sub", "ordered_on::timestamp >= start_date AND ordered_on::timestamp < end_date") // Join the subquery on date conditions
      .addGroupBy("start_date") // Group results by start date
      .addGroupBy("end_date") // Group results by end date
      .addOrderBy("start_date") // Order results by start date
      .setParameters({ // Set parameters for the query
        start_date: startingDate, // Start date for filtering
        end_date: endingDate, // End date for filtering
        store_id: this.store_id, // Store ID of the logged-in user
        ...subqueryForTimeSeries.getParameters() // Include any additional parameters from the subquery
      })
      .getRawMany(); // Execute the query and return multiple raw results
  }

  /**
   * @description Get customers count
   * @param customerQueryBuilder SelectQueryBuilder<LineItem | Order>
   * @param startDate string
   * @param endDate string
   * @param forNewCustomer boolean
   * @returns Promise<{ customers: number }>
   */
  protected async getCustomersCount(customerQueryBuilder: SelectQueryBuilder<LineItem | Order>, startDate: string, endDate: string, forNewCustomer: boolean): Promise<{ customers: number }> {
    // cutomer self join query builder to get new and repeated customers count based on the previous orders
    const customerSelfJoinQueryBuilder = this.manager_.createQueryBuilder()
      .select("CAST(COUNT(DISTINCT o1.customer_id) AS DOUBLE PRECISION) AS customers")
      .from("(" + customerQueryBuilder.getQuery() + ")", "o1")
      .leftJoin("(" + customerQueryBuilder.getQuery() + ")", "o2", "o2.customer_id = o1.customer_id AND o2.created_at::timestamp < :start_date")
      .where("o1.created_at::timestamp >= :start_date AND o1.created_at::timestamp <= :end_date")

    if (forNewCustomer) {
      customerSelfJoinQueryBuilder
        .andWhere("o2.customer_id IS NULL")
    }
    else {
      customerSelfJoinQueryBuilder
        .andWhere("o2.customer_id IS NOT NULL")
    }

    return await customerSelfJoinQueryBuilder
      .setParameters({
        start_date: startDate,
        end_date: endDate,
        ...customerQueryBuilder.getParameters()
      })
      .getRawOne()
  }

  /**
   * @description Get customers time-series
   * @param subqueryForTimeSeries SelectQueryBuilder<LineItem | Order>
   * @param timeFrame "hour" | "day" | "week" | "month"
   * @param startDate string
   * @param endDate string
   * @param forNewCustomer boolean
   * @returns Promise<{ start_date: Date, end_date: Date, customers: number }[]>
   */
  protected async getCustomersTimeSeries(subqueryForTimeSeries: SelectQueryBuilder<LineItem | Order>, timeFrame: TimeFrame, startDate: string, endDate: string, forNewCustomer: boolean): Promise<{ start_date: Date, end_date: Date, customers: number }[]> {
    // subquery to get customers in given date range of time-series
    const customerTimeSeriesSubQuery1 = this.manager_.createQueryBuilder()
      .select([
        "start_date",
        "sub.customer_id as customer_id",
      ])
      .from("(" + this.getTimeSeriesSubquery(timeFrame) + ")", "ds")
      .leftJoin("(" + subqueryForTimeSeries.getQuery() + ")", "sub", "created_at::timestamp >= start_date AND created_at::timestamp < end_date")

    /**
     * This is the Subquery to get new and repeated customer counts in a given date range of time-series,
     * this will only return records that have the customer count,
     * the remaining records with 0 will be filled using the COALESCE and that will use an extra right join with time-series
     */
    const customerTimeSeriesSubQuery2 = this.manager_.createQueryBuilder()
      .select([
        "start_date",
        "COUNT(DISTINCT sub1.customer_id) as customers"
      ])
      .from("(" + customerTimeSeriesSubQuery1.getQuery() + ")", "sub1")
      // .leftJoin("(" + subqueryForTimeSeries.getQuery() + ")", "sub2", "sub2.created_at::timestamp < start_date AND sub1.customer_id = sub2.customer_id")

    if (forNewCustomer) {
      customerTimeSeriesSubQuery2
        .leftJoin("(" + subqueryForTimeSeries.getQuery() + ")", "sub2", "sub2.created_at::timestamp < start_date AND sub1.customer_id = sub2.customer_id")
        .where("sub2.order_id IS NULL")
        .groupBy("start_date")
    } else {
      customerTimeSeriesSubQuery2
        // .where("sub2.order_id IS NOT NULL")
        .groupBy("start_date")
        .addGroupBy("sub1.customer_id")
        .having("COUNT(sub1.customer_id) > 1")
    }

    // use result of the customerTimeSeriesSubQuery2 with date-series to get all the records in the time-series for customers count
    return await this.manager_.createQueryBuilder()
      .select([
        "ds.start_date",
        "ds.end_date",
        "CAST(COALESCE(sub.customers, 0) AS DOUBLE PRECISION) as customers"
      ])
      .from("(" + this.getTimeSeriesSubquery(timeFrame) + ")", "ds")
      .leftJoin("(" + customerTimeSeriesSubQuery2.getQuery() + ")", "sub", "ds.start_date = sub.start_date")
      .orderBy("start_date")
      .setParameters({
        start_date: startDate,
        end_date: endDate,
        payment_status: PAYMENT_STATUS_TO_INCLUDE_IN_REVENUE,
        store_id: this.store_id,
        ...subqueryForTimeSeries.getParameters()
      })
      .getRawMany()
  }

  /**
   * Returns repeat customer time series using a raw SQL query.
   * @param subqueryForTimeSeries - The query builder for time series (to extract parameters).
   * @param startDate - Start date as string.
   * @param endDate - End date as string.
   * @returns Promise<{ start_date: Date, end_date: Date, customers: number }[]>
   */
  /**
   * Returns repeat customer time series using a raw SQL query.
   * @param subqueryForTimeSeries - The query builder for time series (to extract parameters).
   * @param timeFrame - "hour" | "day" | "week" | "month"
   * @param startDate - Start date as string.
   * @param endDate - End date as string.
   * @returns Promise<{ start_date: Date, end_date: Date, customers: number }[]>
   */
  protected async getRepeatCustomersTimeSeries(
    subqueryForTimeSeries: SelectQueryBuilder<LineItem | Order>,
    timeFrame: TimeFrame,
    startDate: string,
    endDate: string,
    type?: string,
    is_new: boolean = false // new argument
  ): Promise<{ start_date: Date, end_date: Date, customers: number, customer_ids?: string[] }[]> {
    const products = subqueryForTimeSeries.getParameters().variantIds || [];
    const paymentStatuses = PAYMENT_STATUS_TO_INCLUDE_IN_REVENUE;
    let paramIdx = 4;
    const paymentStatusPlaceholders = paymentStatuses.map(() => `$${paramIdx++}`).join(", ");
    const productsPlaceholders = products.length
      ? products.map(() => `$${paramIdx++}`).join(", ")
      : "";
    const createdAtStartIdx = paramIdx++;
    const createdAtEndIdx = paramIdx++;
    const paymentStatus2Placeholders = paymentStatuses.map(() => `$${paramIdx++}`).join(", ");

    // Map timeFrame to SQL interval and date_trunc
    let interval = "1 hour";
    let trunc = "hour";
    if (timeFrame === "day") {
      interval = "1 day";
      trunc = "day";
    } else if (timeFrame === "week") {
      interval = "1 week";
      trunc = "week";
    } else if (timeFrame === "month") {
      interval = "1 month";
      trunc = "month";
    }

    // Build type-specific conditions
    let typeCondition = "";
    if (type) {
      if (type === 'upsell') {
        typeCondition = `AND EXISTS (
          SELECT 1 FROM public.line_item li2
          WHERE li2.order_id = o.id AND li2.metadata->>'is_upsell' = 'true'
          ${products.length ? `AND li2.variant_id IN (${productsPlaceholders})` : ""}
        )`;
      } else if (type === 'services') {
        typeCondition = `AND li.metadata->>'is_service' = 'true'`;
      } else if (type === 'courses') {
        typeCondition = `AND li.metadata->>'is_course' = 'true'`;
      } else if (type === 'invoices') {
        typeCondition = `AND o.cart_id IN (SELECT cart_id FROM public.draft_order WHERE invoice_id IS NOT NULL)`;
      } else if (type === 'tiered_product') {
        typeCondition = `AND p.is_tired_product = true`;
      } else {
        typeCondition = `AND o.cart_id IN (SELECT cart_id FROM public.order WHERE purchase_option = $${paramIdx++}) AND p.is_tired_product = false`;
      }
    }

    // Build repeat/non-repeat customer condition
    let repeatCondition = "";
    if (!is_new) {
      repeatCondition = `
        (
          -- Has ordered previously (before current window)
          EXISTS (
            SELECT 1
            FROM public.order o2
            WHERE o2.customer_id = o.customer_id
            AND o2.store_id = o.store_id
            AND o2.payment_status IN (${paymentStatus2Placeholders})
            AND o2.created_at < $${createdAtStartIdx}
          )
          OR
          -- Has placed multiple orders in the current window (same day)
          (
            SELECT COUNT(*)
            FROM public.order o3
            INNER JOIN public.line_item li3 ON li3.order_id = o3.id
            LEFT JOIN public.product_variant pv3 ON pv3.id = li3.variant_id
            WHERE o3.customer_id = o.customer_id
            AND o3.store_id = o.store_id
            AND o3.payment_status IN (${paymentStatus2Placeholders})
            ${products.length ? `AND pv3.id IN (${productsPlaceholders})` : ""}
            AND (o3.created_at AT TIME ZONE 'IST') >= $${createdAtStartIdx}
            AND (o3.created_at AT TIME ZONE 'IST') < $${createdAtEndIdx}
            AND li3.metadata->>'is_upsell' IS NULL
          ) > 1
        )
      `;
    } else {
      repeatCondition = `
        (
          -- Has NOT ordered previously (before current window)
          NOT EXISTS (
            SELECT 1
            FROM public.order o2
            WHERE o2.customer_id = o.customer_id
            AND o2.store_id = o.store_id
            AND o2.payment_status IN (${paymentStatus2Placeholders})
            AND o2.created_at < $${createdAtStartIdx}
          )
          AND
          -- Has placed only one order in the current window (same day)
          (
            SELECT COUNT(*)
            FROM public.order o3
            WHERE o3.customer_id = o.customer_id
            AND o3.store_id = o.store_id
            AND o3.payment_status IN (${paymentStatus2Placeholders})
            AND o3.created_at >= $${createdAtStartIdx}
            AND o3.created_at < $${createdAtEndIdx}
          ) >= 1
        )
      `;
    }

    const sql = `
      SELECT 
      ds.start_date, 
      ds.end_date, 
      CAST(COALESCE(sub.customers, 0) AS DOUBLE PRECISION) as customers,
      COALESCE(sub.customer_ids, ARRAY[]::text[]) as customer_ids
      FROM (
      SELECT 
        start_time AS start_date,
        start_time + interval '${interval}' AS end_date
      FROM generate_series(
        $1::timestamptz,
        $2::timestamptz,
        interval '${interval}'
      ) AS start_time
      ) ds
      LEFT JOIN (
      SELECT 
        date_trunc('${trunc}', o.created_at AT TIME ZONE 'IST') AS start_date,
        COUNT(DISTINCT o.customer_id) AS customers,
        ARRAY_AGG(DISTINCT o.customer_id) AS customer_ids
      FROM public.order o
      INNER JOIN public.line_item li ON li.order_id = o.id
      LEFT JOIN public.product_variant pv ON pv.id = li.variant_id
      LEFT JOIN public.draft_order dro ON dro.order_id = o.id
      LEFT JOIN public.product p ON p.id = pv.product_id
      WHERE o.store_id = $3
        AND o.payment_status IN (${paymentStatusPlaceholders})
        ${products.length ? `AND pv.id IN (${productsPlaceholders})` : ""}
        AND (o.created_at AT TIME ZONE 'IST') >= $${createdAtStartIdx}
        AND (o.created_at AT TIME ZONE 'IST') < $${createdAtEndIdx}
        ${typeCondition}
        AND ${repeatCondition}
      GROUP BY start_date
      ) sub
      ON ds.start_date = sub.start_date
      ORDER BY ds.start_date ASC
    `;

    // Build params array
    const params = [
      startDate,
      endDate,
      this.store_id,
      ...paymentStatuses,
      ...products,
      startDate,
      endDate,
      ...paymentStatuses,
    ];
    if (type && !['upsell', 'services', 'courses', 'invoices', 'tiered_product'].includes(type)) {
      params.push(type);
    }

    return await this.manager_.query(sql, params);
  }
  /*
   * Retrieves the total views time series data for a specified date range and time frame.
   * This function constructs a query to count views over defined time intervals.
   *
   * @param {SelectQueryBuilder<Cart>} subqueryForTimeSeries - The query builder instance for time series data.
   * @param {TimeFrame} timeFrame - The time frame for aggregating views (e.g., daily, weekly).
   * @param {string} startDate - The start date for the time series data.
   * @param {string} endDate - The end date for the time series data.
   * @returns {Promise<{ start_date: Date; end_date: Date; views: number }[]>} - A promise that resolves to an array of time series data objects.
   */
  protected async getTotalViewsTimeseries(
    subqueryForTimeSeries: SelectQueryBuilder<Cart>,
    timeFrame: TimeFrame,
    startDate: string,
    endDate: string
  ): Promise<{ start_date: Date; end_date: Date; views: number }[]> {

    return await this.manager_.createQueryBuilder()
      .select([
        "ds.start_date AS start_date", // Select the start date of the time series
        "ds.end_date AS end_date",       // Select the end date of the time series
        "COALESCE(COUNT(DISTINCT sub.cart_id), 0) AS views" // Count the number of views, defaulting to 0 if null
      ])
      .from(`(${this.getTimeSeriesSubquery(timeFrame)})`, "ds") // Use the time series subquery to define intervals
      .leftJoin(`(${subqueryForTimeSeries.getQuery()})`, "sub", "sub.created_at::timestamp >= ds.start_date AND sub.created_at::timestamp < ds.end_date") // Left join to count views within each time interval
      .groupBy("ds.start_date, ds.end_date") // Group results by start and end dates
      .orderBy("ds.start_date") // Order results by start date
      .setParameters({    // Set the parameters required i nthe query to retrieve the appropriate data for page views time series
        start_date: startDate, // Set the start date parameter for the query
        end_date: endDate,     // Set the end date parameter for the query
        ...subqueryForTimeSeries.getParameters(), // Include any additional parameters from the original subquery
      })
      .getRawMany(); // Retrieve multiple rows containing the time series data
  }

  /**
   * Note: 
   *  The `revenue-per-product` includes a discount also as of now the discount is applied to the order, not a particular on a particular order item.
   *  So, we can't subtract the discount from the particular line-item price.
   *  We can update this in the future once we have a discount per line-item.
   */
  /**
   * @description Get revenue per product for top 5 products
   * @param params RevenuePerProductInput
   * @returns Promise<RevenuePerProduct[]>
   */
  async revenuePerProduct(params: RevenuePerProductInput): Promise<RevenuePerProduct[]> {
    const { store_id, start_date, end_date, type, user_ids } = params // defining user_ids as new parameter in this api to get data on the basis of user ids shared

    const startDate = format(startOfDay(parseISO(start_date)), DATE_FORMATS.iso8601)
    const endDate = format(endOfDay(parseISO(end_date)), DATE_FORMATS.iso8601)

    // subquery to use while generating time-series data for top products
    const subQuery = this.manager_.createQueryBuilder()
      .select([
        "li.quantity AS quantity",
        "li.unit_price AS unit_price",
        "pv.product_id AS product_id",
        "o.created_at AS created_at"
      ])
      .from(LineItem, "li")
      .innerJoin(Order, "o", "li.order_id = o.id AND o.store_id = :store_id AND payment_status IN (:...payment_status)")
      .innerJoin(ProductVariant, "pv", "pv.id = li.variant_id")
      .innerJoin(Product, "p", "p.id = pv.product_id")

    // fetch top 5 products in the given timeframe by revenue
    const topProductsQuery = this.manager_.createQueryBuilder()
      .select([
        "p.id AS id",
        "p.title AS title",
        "CAST(CAST(SUM(CAST(li.quantity AS NUMERIC)*CAST(li.unit_price AS NUMERIC)) AS NUMERIC)/100 AS DOUBLE PRECISION) AS revenue",
      ])
      .from(LineItem, "li")
      .innerJoin(Order, "o", "o.id = li.order_id")
      .leftJoin(ProductVariant, "pv", "li.variant_id = pv.id")
      .innerJoin(Product, "p", "p.id = pv.product_id")
      .where("o.store_id = :store_id", { store_id })
      .andWhere("o.payment_status IN (:...payment_status)", { payment_status: PAYMENT_STATUS_TO_INCLUDE_IN_REVENUE })
      .andWhere("o.created_at::timestamp >= :start_date AND o.created_at::timestamp <= :end_date", {
        start_date: startDate,
        end_date: queryEndDate(startDate, endDate)  // Using the function to filter the end date as per the input dates
      })

    // fetching data on the user ids basis if user_ids parameter passed in the query
    if (user_ids != undefined && user_ids != "") {
      topProductsQuery.andWhere('p.store_id in (select store_id from "user" where crm_user_id in (' + user_ids + '))')
      subQuery.where('pv.product_id in (select id from product where store_id in (select store_id from "user" where crm_user_id in (' + user_ids + ')))')
    }

    if (type === ORDER_PURCHASE_OPTIONS_ENUM.subscription) {
      topProductsQuery.andWhere("o.purchase_option = :purchase_option", { purchase_option: type })
      subQuery.where("o.purchase_option = :purchase_option", { purchase_option: type })
    }
    else if (type === ORDER_PURCHASE_OPTIONS_ENUM.onetime) {
      topProductsQuery.andWhere("(o.purchase_option = :purchase_option OR o.purchase_option IS NULL)", { purchase_option: type })
      subQuery.where("(o.purchase_option = :purchase_option OR o.purchase_option IS NULL)", { purchase_option: type })
    } 
    else if (type === ORDER_PURCHASE_OPTIONS_ENUM.tiered_product) {
      topProductsQuery.andWhere("p.is_tired_product = 'true'")
      subQuery.where("p.is_tired_product = 'true'")
    }

    topProductsQuery
      .groupBy("p.id")
      .addGroupBy("p.title")
      .orderBy("revenue", "DESC")
    // .limit(5)

    const topProducts = (await topProductsQuery.getRawMany()) as TopProducts[]

    const topProductsId = topProducts.map(product => product.id)
    const timeFrame = getTimeFrame(parseISO(start_date), parseISO(end_date))

    if (topProductsId.length === 0) {
      return []
    }

    // product X date-series subquery
    const productDates = this.manager_.createQueryBuilder()
      .select([
        "p.id AS product_id",
        "start_date",
        "end_date"
      ])
      .from("(" + this.getTimeSeriesSubquery(timeFrame) + ")", "date_series")
      .from(Product, "p")
      .where("p.id IN (:...top_products_id)")

    // final time series query
    const timeSeriesQuery = this.manager_.createQueryBuilder()
      .select([
        "pd.product_id",
        "pd.start_date",
        "pd.end_date",
        "COALESCE(CAST(CAST(SUM(CAST(subq.quantity AS NUMERIC)*CAST(subq.unit_price AS NUMERIC)) AS NUMERIC)/100 AS DOUBLE PRECISION), 0) AS revenue"
      ])
      .from("(" + productDates.getQuery() + ")", "pd")
      .leftJoin("(" + subQuery.getQuery() + ")", "subq", "pd.product_id = subq.product_id AND subq.created_at >= pd.start_date AND subq.created_at < pd.end_date")
      .groupBy("pd.product_id")
      .addGroupBy("pd.start_date")
      .addGroupBy("pd.end_date")
      .orderBy("product_id")
      .addOrderBy("start_date")
      .setParameters({
        start_date: startDate,
        end_date: endDate,
        store_id,
        payment_status: PAYMENT_STATUS_TO_INCLUDE_IN_REVENUE,
        top_products_id: topProductsId,
        ...subQuery.getParameters()
      })

    const timeSeriesData = (await timeSeriesQuery.getRawMany()) as ProductsTimeSeriesData[]

    const revenuePerProduct = topProducts.map(product => {
      const timeSeries = timeSeriesData.filter(tsd => tsd.product_id === product.id).map(tsd => {
        return {
          start: format(tsd.start_date, DATE_FORMATS.iso8601),
          end: format(subMilliseconds(tsd.end_date, 1), DATE_FORMATS.iso8601EOD),
          amount: tsd.revenue
        }
      })
      return {
        productId: product.id,
        productName: product.title,
        amount: product.revenue,
        timeseries: timeSeries
      }
    })

    return revenuePerProduct
  }

  /**
   * @description Get new customer stats for the given time frame
   * @param config FindNewCustomersStatsConfig
   * @returns Promise<NewCustomerStatsResponse>
   */
  async newCustomerStats(config: FindNewCustomersStatsConfig) {
    const { start_date, end_date } = config

    const startDate = format(startOfDay(parseISO(start_date)), DATE_FORMATS.iso8601)
    const endDate = format(endOfDay(parseISO(end_date)), DATE_FORMATS.iso8601)

    // get time frame for the given date range
    const timeFrame = getTimeFrame(parseISO(start_date), parseISO(end_date))

    // sub-query to use in the time-series calculations
    const queryForTimeSeries = this.manager_.createQueryBuilder()
      .select([
        "o.id AS order_id",
        "o.created_at AS created_at",
        "o.customer_id AS customer_id"
      ])
      .from(Order, "o")
      .where("o.store_id = :store_id AND o.payment_status IN (:...payment_status)")
      .setParameters({
        start_date: startDate,
        end_date: endDate,
        payment_status: PAYMENT_STATUS_TO_INCLUDE_IN_REVENUE,
        store_id: this.loggedInUser_.store_id
      })

    // query builder to get new customers count
    const queryForNewCustomers = this.manager_.createQueryBuilder()
      .select([
        "customer_id",
        "o.created_at AS created_at"
      ])
      .from(Order, "o")
      .where("o.store_id = :store_id AND o.payment_status IN (:...payment_status)")
      .setParameters({
        start_date: startDate,
        end_date: endDate,
        payment_status: PAYMENT_STATUS_TO_INCLUDE_IN_REVENUE,
        store_id: this.store_id
      })

    const [newCustomers, timeSeries] = await Promise.all([
      this.getCustomersCount(queryForNewCustomers, startDate, endDate, true),
      this.getCustomersTimeSeries(queryForTimeSeries, timeFrame, startDate, endDate, true)
    ])

    return {
      start_date: startDate,
      end_date: endDate,
      new_customers: newCustomers.customers,
      time_series: timeSeries.map(tsd => ({
        start: format(tsd.start_date, DATE_FORMATS.iso8601),
        end: format(subMilliseconds(tsd.end_date, 1), DATE_FORMATS.iso8601EOD),
        value: tsd.customers
      }))
    }
  }

  /** 
   * @description Retrieve a list of top 5 products by revenue
   * @param params FindTopProductsConfig
   * @returns Promise<TopProduct[]>
   */
  async listTopProducts(params: FindTopProductsConfig): Promise<TopProduct[]> {
    const { start_date, end_date, type } = params

    // get start and end date
    const startDate = format(startOfDay(parseISO(start_date)), DATE_FORMATS.iso8601)
    const endDate = format(endOfDay(parseISO(end_date)), DATE_FORMATS.iso8601)

    const queryBuilder = this.manager_.createQueryBuilder()

    // filter products with sales between start and end date
    queryBuilder.select([
      "p.id AS id",
      "p.title AS title",
      "p.thumbnail AS thumbnail",
      "p.cropped_thumbnail AS cropped_thumbnail",
      "p.created_at AS created_at",  // Corrected the SQL syntax (UPPERCASE)
      "li.unit_price AS price",
      "CAST(CAST(SUM(CAST(li.quantity AS NUMERIC)*CAST(li.unit_price AS NUMERIC)) AS NUMERIC)/100 AS DOUBLE PRECISION) AS revenue",
      "CAST(COUNT(DISTINCT o.id) AS DOUBLE PRECISION) AS sales",
      "CAST(CAST(COUNT(DISTINCT o.id) AS DOUBLE PRECISION)/:days AS DOUBLE PRECISION) AS avg_sales_per_day",
    ])
      .from(LineItem, "li")
      .innerJoin(Order, "o", "o.id = li.order_id")
      .leftJoin(ProductVariant, "pv", "li.variant_id = pv.id")
      .innerJoin(Product, "p", "p.id = pv.product_id")
      .innerJoin(Cart, "c", "c.id = o.cart_id"); // Ensure Cart is joined for all cases, specifically for CASE CONVERSIONRATE
      
    if (type == REPORTS_ORDER_TYPE_ENUM.INVOICE) queryBuilder.innerJoin(DraftOrder, "dro", "dro.order_id=o.id");
      
      queryBuilder.where("o.store_id = :store_id", { store_id: this.loggedInUser_.store_id })
      .andWhere("o.payment_status IN (:...payment_status)", { payment_status: PAYMENT_STATUS_TO_INCLUDE_IN_REVENUE })
      .andWhere("o.created_at::timestamp >= :start_date AND o.created_at::timestamp <= :end_date", {
        start_date: startDate,
        end_date: queryEndDate(startDate, endDate)  // Using the function to filter the end date as per the input dates
      })
      .setParameter("days", differenceInDays(parseISO(endDate), parseISO(startDate)) + 1);

    // Filter products based on the type of order
    switch (type) {
      case REPORTS_ORDER_TYPE_ENUM.SUBSCRIPTION:
        queryBuilder.andWhere("o.purchase_option = :purchase_option AND p.is_tired_product = 'false'", { purchase_option: type })
        break
      case REPORTS_ORDER_TYPE_ENUM.ONETIME:
        queryBuilder.andWhere("(o.purchase_option = :purchase_option OR o.purchase_option IS NULL)", { purchase_option: type })
        break
      case REPORTS_ORDER_TYPE_ENUM.TIEREDPRODUCT:
        queryBuilder.andWhere("o.purchase_option = :purchase_option AND p.is_tired_product = 'true'", { purchase_option: 'subscription' })
        break  
      case REPORTS_ORDER_TYPE_ENUM.COURSE:
        queryBuilder.andWhere("li.metadata->>'is_course' = 'true'")
        break
      case REPORTS_ORDER_TYPE_ENUM.SERVICE:
        queryBuilder.andWhere("li.metadata->>'is_service' = 'true'")
        break
      case REPORTS_ORDER_TYPE_ENUM.INVOICE:
        queryBuilder.andWhere("dro.invoice_id IS NOT NULL")
        break
      case REPORTS_ORDER_TYPE_ENUM.UPSELL:
        queryBuilder.andWhere("li.metadata->>'is_upsell' = 'true'")
        break
      // Added a new case for the data to be specifically retrieved for the Page Views
      case REPORTS_ORDER_TYPE_ENUM.PAGEVIEWS:
        queryBuilder.andWhere(`o.page_id = c.metadata->>'pageId'`) // Filter results to include only orders matching the specific page ID from the cart metadata
        break
      case REPORTS_ORDER_TYPE_ENUM.CONVERSIONRATE:  // Added a CASE to filter the top products per Conversion Rate
        queryBuilder.andWhere(`o.page_id = c.metadata->>'pageId'`); // Filter orders to match the page ID from cart metadata
        break;
      //added an additional condition to handle vt case
     case REPORTS_ORDER_TYPE_ENUM.VIRTUALTERMINAL:
        queryBuilder.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true'`);
        break;
      /** Additional case for payment mode */
      case REPORTS_ORDER_TYPE_ENUM.VIRTUALTERMINALACH:
        queryBuilder.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'ach' OR o.metadata->> 'transactionType' = 'ach')`)
        break;
      case REPORTS_ORDER_TYPE_ENUM.VIRTUALTERMINALCASH:
        queryBuilder.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'cash' OR o.metadata->> 'transactionType' = 'cash')`)
        break;
      case REPORTS_ORDER_TYPE_ENUM.VIRTUALTERMINALCARD:
        queryBuilder.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'card' OR o.metadata->> 'transactionType' = 'card')`)
        break;
      case REPORTS_ORDER_TYPE_ENUM.VIRTUALTERMINALCLOVER:
        queryBuilder.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'clover' OR o.metadata->> 'transactionType' = 'clover')`);
        break;
      case REPORTS_ORDER_TYPE_ENUM.VIRTUALTERMINALDEJAVOO:
        queryBuilder.andWhere(`o.metadata->> 'is_virtual_terminal_order' = 'true' AND (o.metadata->> 'payment_mode' = 'deja_voo' OR o.metadata->> 'transactionType' = 'deja_voo')`)
        break

      default:
        // No specific adjustments for other order types
        break
    }

    // Handle ordering of results based on the report type
    switch (type) {
      case REPORTS_ORDER_TYPE_ENUM.PAGEVIEWS:
        // Order results by the count of ordered pages in descending order
        queryBuilder.orderBy("ordered_page_count", "DESC")
        break
      case REPORTS_ORDER_TYPE_ENUM.CONVERSIONRATE:
        // Order results by the conversion percentage count in descending order
        queryBuilder.orderBy("conversion_percentage", "DESC"); // Sort by conversion percentage in descending order
        break
      default:
        // For other types, order results by revenue in descending order
        queryBuilder.orderBy("revenue", "DESC")
        break
    }

    queryBuilder
      .groupBy("p.id, p.title, p.thumbnail, p.created_at, li.unit_price") // Grouping all non-aggregated columns to ensure accurate aggregation
      .limit(5); // Limit the results to the top 5 products

    const topProducts = await queryBuilder.getRawMany() as TopProduct[];

    // Get repository instance (adjust as per your DI/container setup)
    const productRepo = ProductRepository;

    // // Now, for each product, get ordered_page_count and conversion_percentage
    // for (const product of topProducts) {
    //   // Build viewsQueryBuilder for this product
    //   const viewsQueryBuilder = this.manager_.createQueryBuilder()
    //     .select([
    //       "c.metadata->>'pageId' AS page_id",
    //       "COUNT(DISTINCT c.metadata->>'pageId') AS view_count"
    //     ])
    //     .from(Cart, "c")
    //     .innerJoin(ProductPage, "pp", "pp.id = c.metadata->>'pageId'")
    //     .where("c.metadata->>'pageId' IS NOT NULL")
    //     .andWhere("c.created_at::timestamp >= :start_date AND c.created_at::timestamp <= :end_date", {
    //       start_date: startDate,
    //       end_date: queryEndDate(startDate, endDate)
    //     })
    //     .andWhere("pp.product_id = :product_id", { product_id: product.id })
    //     .groupBy("c.metadata->>'pageId'");

    //   const viewsResult = await this.getTotalViewsCount(viewsQueryBuilder, startDate, queryEndDate(startDate, endDate));
    //   product.ordered_page_count = Number(viewsResult.views);

    //   product.conversion_percentage = product.revenue / product.ordered_page_count || 0; // Calculate conversion percentage as revenue divided by ordered page count
      
    // }
    // Get revenue using analytics function
    for (const product of topProducts) {
      // Get revenue using analytics function
      const analytics = await productRepo.getAnalytics(
        this.loggedInUser_.store_id,
        product.id,
        startDate,
        queryEndDate(startDate, endDate)
      );
      product.revenue = analytics[0]?.revenue || 0;

      let storeOrderStats = await this.storeOrderStats({
        store_id: this.loggedInUser_.store_id,
        products: [product.id],
        start_date: startDate,
        end_date: endDate
      });
      // Get page views using analytics function
      product.ordered_page_count = storeOrderStats?.page_views?.current_value || 0;

      // Calculate conversion percentage
      product.conversion_percentage = storeOrderStats?.conversion_rate?.current_value || 0;
    }
    // Sort again by revenue descending in case analytics revenue is different
    topProducts.sort((a, b) => Number(b.revenue) - Number(a.revenue));
    return topProducts;
  }

  /**
   * @description Returns the orders created in the last 24 hours for a store and
   * the failed payments count and order item count as well.
   * @param config: FindRecentOrdersConfig
   * @returns: Promise<[Record<string, string | number | Date>[], number, number]>
  */
  async getOrderHistory(config: FindRecentOrdersConfig): Promise<[Record<string, string | number | Date>[], number, number]> {
    const { store_id, offset, limit } = config
    // changes done as per the modules updated
    // const orderRepo = this.manager_.withRepository(this.orderRepository_)
    this.manager_.createQueryBuilder()
      .select([
        "li.quantity AS quantity",
        "li.unit_price AS unit_price",
        "pv.product_id AS product_id",
        "o.created_at AS created_at"
      ])
      .from(LineItem, "li")
      .innerJoin(Order, "o", "li.order_id = o.id AND o.store_id = :store_id AND payment_status IN (:...payment_status)")
      .innerJoin(ProductVariant, "pv", "pv.id = li.variant_id")
      .innerJoin(Product, "p", "p.id = pv.product_id")
      .leftJoin(DraftOrder, "dro", "dro.order_id = o.id")

    const orderListQuery = this.manager_.createQueryBuilder()
      .from(Order, "order")
      .leftJoin("order.customer", "customer")
      .leftJoin("order.payments", "payments")
      .leftJoin("order.items", "items")
      .leftJoin(DraftOrder, "dro", "dro.order_id = order.id")
      .loadRelationCountAndMap("order.order_item_count", "order.items")
      .select([
        "order.id",
        "order.created_at",
        "order.payment_status",
        "order.status",
        "order.display_id",
        "order.purchase_option",
        "customer.id",
        "customer.first_name",
        "customer.last_name",
        "customer.email",
        "payments.id",
        "payments.amount",
        "payments.raw_amount",
        "payments.currency_code",
      ])
      .where(`order.created_at >= NOW() - '1 day'::INTERVAL`)
      .orderBy('order.created_at', 'DESC')  // Corrected orderBy clause
      .andWhere('order.store_id = :store_id', { store_id })
      // .andWhere([
      //   {
      //     purchase_option: ORDER_PURCHASE_OPTIONS_ENUM.onetime,
      //   },
      //   {
      //     draft_order_id: IsNull(),
      //     purchase_option: ORDER_PURCHASE_OPTIONS_ENUM.subscription,
      //   }
      // ]);

    // Query to fetch payment logs created in the last 24 hours
    const paymentLogQuery = this.manager_.createQueryBuilder()
      .from(PaymentLog, "payment_log")
      .select([
        'payment_log.id',
        'payment_log.created_at',
        'payment_log.status',
        'payment_log.customer_details',
        'payment_log.payment_log',
        'payment_log.cart_id',
        'payment_log.display_id',
        'payment_log.amount'
      ])
      .where(`payment_log.created_at >= NOW() - '1 day'::INTERVAL`)
      .andWhere('payment_log.store_id = :store_id', { store_id })
      .orderBy('payment_log.created_at', 'DESC');

    // Execute both order list and payment log queries in parallel
    const [orders, paymentLogs] = await Promise.all([
      (await orderListQuery.skip(offset).take(limit).getMany()) as ReportOrders[],
      paymentLogQuery.getMany()
    ]);

    // Get the total count of orders
    const totalOrdersCount = await orderListQuery.getCount();
    // Count the number of failed payments
    const failedPayments = paymentLogs.length;

    // Map the orders to a new format
    const updatedOrderList = await Promise.all(orders.map(async order => {
    // 1. Get subtotal
    const subtotalResult = await this.manager_.query(
      `SELECT COALESCE(SUM(li.unit_price * li.quantity), 0) AS subtotal
      FROM line_item li WHERE li.order_id = $1`,
      [order.id]
    );
    const subtotal = Number(subtotalResult[0]?.subtotal) || 0;

    // 2. Get gift card amount from gift_card_transaction
    const giftCardTxResult = await this.manager_.query(
      `SELECT COALESCE(SUM(amount), 0) AS gift_card_amount
      FROM gift_card_transaction WHERE order_id = $1`,
      [order.id]
    );
    const giftCardAmount = Number(giftCardTxResult[0]?.gift_card_amount) || 0;

    // 3. Subtotal after gift card
    const afterGiftCard = subtotal - giftCardAmount;

    // 4. Get discount total
    const discountResult = await this.manager_.query(
      `SELECT COALESCE(SUM(
        CASE
          WHEN dr.type = 'percentage' THEN $2 * (dr.value / 100)
          ELSE dr.value
        END
      ), 0) AS discount_total
      FROM order_discounts od
      LEFT JOIN discount d ON od.discount_id = d.id
      LEFT JOIN discount_rule dr ON d.rule_id = dr.id
      WHERE od.order_id = $1`,
      [order.id, afterGiftCard]
    );
    const discountTotal = Number(discountResult[0]?.discount_total) || 0;

    // 5. Subtotal after discount
    const afterDiscount = afterGiftCard - discountTotal;

    // 6. Get tax total (apply on after discount)
    const taxResult = await this.manager_.query(
      `SELECT ROUND(CAST(COALESCE(SUM(
        (
          (li.unit_price * li.quantity)
          - ((li.unit_price * li.quantity) / NULLIF($2, 0) * $3) -- proportional gift card deduction
          - ((li.unit_price * li.quantity) / NULLIF($4, 0) * $5) -- proportional discount deduction
        ) * (litl.rate / 100)
      ), 0) AS NUMERIC), 2) AS tax_total
      FROM line_item li
      LEFT JOIN line_item_tax_line litl ON li.id = litl.item_id
      WHERE li.order_id = $1`,
      [order.id, subtotal, giftCardAmount, afterGiftCard, discountTotal]
    );

    let taxTotal = Number(taxResult[0]?.tax_total) || 0;
    if (taxTotal < 0) taxTotal = 0;

    // 7. Get dual price (active_fee_amount)
    const dualPriceResult = await this.manager_.query(
      `SELECT COALESCE(
        CASE
          WHEN ocw.dp_settings::jsonb ? 'active_fee_amount'
          THEN NULLIF(ocw.dp_settings->>'active_fee_amount','')::numeric
          ELSE 0::numeric
        END, 0) AS dual_price_amount
        FROM order_cart_wise_dp_settings ocw
        LEFT JOIN draft_order do1 ON do1.id = ocw.draft_order_id
        WHERE do1.order_id = $1
        LIMIT 1`,
      [order.id]
    );
    const dualPriceAmount = Number(dualPriceResult[0]?.dual_price_amount) || 0;

    // 8. Get tips
    const tipsResult = await this.manager_.query(
      `SELECT COALESCE(tips_amount, 0) AS tips_amount FROM "order" WHERE id = $1`,
      [order.id]
    );
    const tipsAmount = Number(tipsResult[0]?.tips_amount) || 0;

    // 9. Get refund total
    const refundResult = await this.manager_.query(
      `SELECT COALESCE(SUM(amount), 0) AS refund_total FROM refund WHERE order_id = $1`,
      [order.id]
    );
    const refundTotal = Number(refundResult[0]?.refund_total) || 0;

    // 10. Final payment amount
    const paymentAmount = Math.max(
      afterDiscount + taxTotal + dualPriceAmount * 100 + tipsAmount - refundTotal,
      0
    );

    const payment = order.payments[0];

    return {
      orderId: order.id,
      orderCreatedAt: order.created_at,
      orderPaymentStatus: order.payment_status,
      orderStatus: order.status,
      orderIdInt: order.display_id,
      orderPurchaseOption: order.purchase_option,
      customerId: order.customer.id,
      customerFirstName: order.customer.first_name,
      customerLastName: order.customer.last_name,
      customerEmail: order.customer.email,
      paymentId: payment?.id,
      paymentCurrencyCode: payment?.currency_code,
      orderItemCount: order?.order_item_count,
      subtotal,
      discountTotal,
      giftCardTotal: giftCardAmount,
      taxTotal,
      dualPriceAmount,
      tipsAmount,
      refundTotal,
      paymentAmount,
    };
  }));

    // Map the payment logs to a new format and fetch customer details
    const updatedPaymentLogList = await Promise.all(paymentLogs.map(async log => {
      const customer = await dataSource.query(`SELECT * FROM "customer" WHERE id in (select customer_id from cart where id = '${log.cart_id}') `);
      return {
        orderId: log.id,
        orderCreatedAt: log.created_at,
        orderPaymentStatus: 'failed',
        orderStatus: 'failed',
        orderIdInt: log.display_id,
        orderPurchaseOption: 'onetime',
        customerId: customer[0]?.id,
        customerFirstName: customer[0]?.first_name || "No",
        customerLastName: customer[0]?.last_name || "Name",
        customerEmail: customer[0]?.email,
        paymentId: log.id,
        paymentAmount: log.amount,
        paymentCurrencyCode: 'usd',
        orderItemCount: 1
      };
    }));

    // Combine and sort the order and payment log lists by creation date
    const response = [...updatedOrderList, ...updatedPaymentLogList].sort((a, b) => new Date(b.orderCreatedAt).getTime() - new Date(a.orderCreatedAt).getTime());

    // Return the combined response, total order count, and failed payments count
    return [response, totalOrdersCount, failedPayments];
  }

  /**
   * Returns the time-series of orders created in the last 24 hours
   * @param store_id: string
   * @returns: Promise<[OrderTimeseries[], number]>
   */
  async getOrderMetrics(store_id: string): Promise<[OrderTimeseries[], number]> {
    const timeSeriesSubquery = this.manager_.createQueryBuilder()
      .select(`generate_series(date_trunc('day', now()), date_trunc('day', now()) + '23 hours 59 minutes'::interval, '1 hour'::interval)`, 'hour')
      .addSelect(`generate_series(date_trunc('day', now()) + '1 hour'::interval, date_trunc('day', now()) + '24 hours 59 minutes'::interval, '1 hour'::interval)`, 'hourplus1')
      .from('(select 1)', 'series');

    const orderMetrics = await this.manager_.createQueryBuilder()
      .select([
        `hour as start`,
        `hourplus1 as end`,
        'COUNT(o.id)::int AS amount'
      ])
      .from(queryBuilder => {
        return queryBuilder
          .select('hour')
          .addSelect('hourplus1')
          .from("(" + timeSeriesSubquery.getQuery() + ")", 'timeSeriesSubquery')
      }, 'hours')
      .leftJoin('order', 'o', `DATE_TRUNC('hour', o.created_at) = hour AND o.store_id = :store_id AND (o.purchase_option = :otp_purchase_option OR (o.draft_order_id IS NULL AND o.purchase_option = :subscription_purchase_option))`)
      .groupBy('hours.hour')
      .addGroupBy('hours.hourplus1')
      .orderBy('hours.hour')
      .setParameters({
        store_id,
        otp_purchase_option: ORDER_PURCHASE_OPTIONS_ENUM.onetime,
        subscription_purchase_option: ORDER_PURCHASE_OPTIONS_ENUM.subscription
      })
      .getRawMany()

    // changes done as per the modules updated
    const totalOrderCount = await this.manager_.createQueryBuilder()
      .from(Order, "order")
      .select('COUNT(order.id)::int')
      .where('order.store_id = :store_id AND order.created_at >= CURRENT_DATE', { store_id })
      .andWhere([
        {
          purchase_option: ORDER_PURCHASE_OPTIONS_ENUM.onetime,
        },
        {
          draft_order_id: IsNull(),
          purchase_option: ORDER_PURCHASE_OPTIONS_ENUM.subscription,
        }
      ])
      .getRawOne()

    return [orderMetrics, totalOrderCount.count]
  }

}

export default ReportsService