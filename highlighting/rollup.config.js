import { lezer } from "@lezer/generator/rollup";

export default {
  input: "./src/pixel.grammar",
  output: [
    {
      format: "es",
      file: "./dist/index.js",
    },
  ],
  external: ["@lezer/lr", "@lezer/highlight"],
  plugins: [lezer()],
};
