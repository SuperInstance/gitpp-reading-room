import { derive } from "./fold.mjs";
let t = ""; process.stdin.on("data", (c) => (t += c)).on("end", () =>
  console.log(JSON.stringify(derive(JSON.parse(t)))));
