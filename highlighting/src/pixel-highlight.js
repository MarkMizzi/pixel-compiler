import { styleTags, tags } from "@lezer/highlight";

export const highlighting = styleTags({
  LineComment: tags.comment,
  BlockComment: tags.comment,

  SimpleTypename: tags.typeName,

  let: tags.keyword,
  __print: tags.keyword,
  __delay: tags.keyword,
  __pixelr: tags.keyword,
  __pixel: tags.keyword,
  __putchar: tags.keyword,
  return: tags.keyword,
  if: tags.keyword,
  else: tags.keyword,
  for: tags.keyword,
  while: tags.keyword,
  fun: tags.keyword,
  __randi: tags.keyword,
  __width: tags.keyword,
  __height: tags.keyword,
  __read: tags.keyword,
  __newarr: tags.keyword,
  __getchar: tags.keyword,
  __float2int: tags.keyword,

  and: tags.operator,
  or: tags.operator,
  not: tags.operator,

  Identifier: tags.name,

  BoolLiteral: tags.literal,
  Colour: tags.string,
  Number: tags.number,

  LeftBrace: tags.punctuation,
  RightBrace: tags.punctuation,
  Semicolon: tags.punctuation,
  Colon: tags.punctuation,
  Equals: tags.operator,
  Comma: tags.punctuation,
  LeftBracket: tags.punctuation,
  RightBracket: tags.punctuation,
  Arrow: tags.punctuation,
  MultiplicativeOp: tags.operator,
  AdditiveOp: tags.operator,
  RelationalOp: tags.operator,
  UnaryOp: tags.operator,
  LeftSqBracket: tags.punctuation,
  RightSqBracket: tags.punctuation,
});
