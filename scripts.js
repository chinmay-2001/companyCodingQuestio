// setTimeout(() => {
//   console.log("hi");
// }, 2000);
import fs from "fs";

const myPromise = new Promise((resolve, reject) => {
  const rand = Math.floor(Math.random() * 2);
  if (rand === 0) {
    resolve();
  } else {
    reject();
  }
});

myPromise
  .then(() => console.log("successfull"))
  .catch(() => console.error("something went wrong"));
