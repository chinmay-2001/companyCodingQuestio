vector<int> segnode, lazynode;

/* clang-format on */
void push_lazy(int node, int start, int end)
{
    if (lazynode[node] != 0)
    {
        segnode[node] = max(segnode[node], lazynode[node]);

        if (start != end)
        {
            lazynode[2 * node] = max(lazynode[2 * node], lazynode[node]);
            lazynode[2 * node + 1] = max(lazynode[2 * node + 1], lazynode[node]);
        }
        lazynode[node] = 0;
    }
}

int query(int node, int start, int end, int l, int r)
{
    push_lazy(node, start, end);
    if (end < l || start > r)
    {
        return 0;
    }

    if (l <= start and end <= r)
    {
        return segnode[node];
    }

    int mid = (start + end) / 2;

    int left = query(2 * node, start, mid, l, r);
    int right = query(2 * node + 1, mid + 1, end, l, r);
    return max(left, right);
}

void update(int node, int l, int r, int val, int start, int end)
{
    push_lazy(node, start, end);
    if (start > r || l > end)
    {
        return;
    }

    if (l <= start and end <= r)
    {
        segnode[node] = max(val, segnode[node]);
        if (start != end)
        {
            lazynode[2 * node] = max(val, lazynode[2 * node]);
            lazynode[2 * node + 1] = max(val, lazynode[2 * node + 1]);
        }
        return;
    }

    int mid = (start + end) / 2;

    update(2 * node, l, r, val, start, mid);

    update(2 * node + 1, l, r, val, mid + 1, end);

    segnode[node] = max(segnode[2 * node], segnode[2 * node + 1]);
    return;
}
