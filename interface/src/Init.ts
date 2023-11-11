import { HttpInit } from "./api/http";

export default function Init() {
    console.log(("BASE_URL: " + process.env.REACT_APP_BASE_URL) as string);
    HttpInit();
}
