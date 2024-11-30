/* CodeMirror extension for supporting Pixel language */

import { LanguageSupport, LRLanguage } from '@codemirror/language'

import { parser } from 'pixel-highlighting'

/**
A language provider
*/
const pixelLanguage = /*@__PURE__*/ LRLanguage.define({
  name: 'xml',
  parser
})

/**
Pixel language support.
*/
export default function pixel(conf = {}) {
  return new LanguageSupport(pixelLanguage, [])
}
