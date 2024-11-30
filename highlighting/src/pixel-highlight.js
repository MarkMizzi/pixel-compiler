import { styleTags, tags } from "@lezer/highlight";

export const highlighting = styleTags({
  LineComment: tags.comment,
  BlockComment: tags.comment,

  SimpleTypename: tags.typeName,

  "let __print __delay __pixelr __pixel return if else for while fun __randi __width __height __read __newarr":
    tags.keyword,
  "and or not": tags.operator,

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
