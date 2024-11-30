import { styleTags, tags } from "@lezer/highlight";

export const highlighting = styleTags({
  LineComment: tags.comment,

  Opcode: tags.keyword,
  FunctionLabel: tags.operator,

  Number: tags.number,
  PCOffset: tags.literal,
  Colour: tags.literal,
});
