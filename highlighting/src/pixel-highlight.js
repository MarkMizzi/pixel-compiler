import { styleTags, tags } from "@lezer/highlight";

export const highlighting = styleTags({
  LineComment: tags.comment,
  BlockComment: tags.comment,

  SimpleTypename: tags.typeName,

  "VariableDecl/LetKeyword": tags.keyword,
  PrintKeyword: tags.keyword,
  DelayKeyword: tags.keyword,
  PixelRKeyword: tags.keyword,
  PixelKeyword: tags.keyword,
  "RtrnStatement/RtrnKeyword": tags.keyword,
  "IfStatement/IfKeyword": tags.keyword,
  "IfStatement/ElseKeyword": tags.keyword,
  "ForStatement/ForKeyword": tags.keyword,
  "WhileStatement/WhileKeyword": tags.keyword,
  "FunctionDecl/FunKeyword": tags.keyword,
  RandIKeyword: tags.keyword,
  HeightKeyword: tags.keyword,
  WidthKeyword: tags.keyword,
  ReadKeyword: tags.keyword,
  NewArrKeyword: tags.keyword,

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
