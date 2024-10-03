import { Config } from 'tailwindcss'

export default {
  content: [
    './src/App.vue',
    './src/components/*.vue',
  ],
  theme: {
    extend: {},
  },
  variants: {
    extend: {},
  },
  plugins: [
    require('@tailwindcss/forms'),
  ],
} satisfies Config