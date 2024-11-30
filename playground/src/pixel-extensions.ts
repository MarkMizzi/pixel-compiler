/* CodeMirror extension for supporting Pixel language */

import { LanguageSupport, LRLanguage } from '@codemirror/language'

import { pixelParser, pixelAsmParser } from 'pixel-highlighting'

/**
Pixel and Pixel ASM language support.
*/
export function pixel() {
  return new LanguageSupport(
    LRLanguage.define({
      name: 'pixel',
      parser: pixelParser
    }),
    []
  )
}

export function pixelAsm() {
  return new LanguageSupport(
    LRLanguage.define({
      name: 'pixel-asm',
      parser: pixelAsmParser
    }),
    []
  )
}
