<template>
  <div class="grid grid-cols-1">
    <div class="flex flex-row h-12 justify-start p-2 gap-x-0">
      <button class="w-32 p-2 bg-slate-900 link-green" @click="onPressCompile()">compile</button>
      <select
        id="select-example-src-code"
        class="w-48 p-2 bg-slate-900 link-green"
        @input="setCodeToExample(($event.target as any).value)"
      >
        <option class="bg-slate-900 link-green" value="">choose an example...</option>
        <option class="bg-slate-900 link-green" value="fibonacci">fibonacci</option>
        <option class="bg-slate-900 link-green" value="race">race</option>
        <option class="bg-slate-900 link-green" value="rule 110">rule 110</option>
        <option class="bg-slate-900 link-green" value="typography">typography</option>
        <option class="bg-slate-900 link-green" value="game of life">game of life</option>
        <option class="bg-slate-900 link-green" value="wall clock">wall clock</option>
        <option class="bg-slate-900 link-green" value="clear">clear</option>
      </select>
    </div>
    <CodeEditor
      ref="srcCodeEditor"
      class="border-2 border-slate-700"
      :readonly="false"
    ></CodeEditor>
  </div>
</template>

<script setup lang="ts">
import { pixel } from '@/pixel-extensions'
import CodeEditor from './CodeEditor.vue'
import { onMounted, ref, type Component, type Ref } from 'vue'

const props = defineProps<{
  compile: (s: string) => void
}>()

// once the component is mounted this will automatically be set to
// point to child component with ref="srcCodeEditor"
const srcCodeEditor: Ref<Component | null> = ref(null)

const onPressCompile = () => {
  props.compile((srcCodeEditor.value as typeof CodeEditor).getCode())
}

// once the component is mounted, we add highlighting for Pixel language
onMounted(() => {
  // add Pixel extension to the code editor.
  ;(srcCodeEditor.value as typeof CodeEditor).addExtension(pixel())
})

// store example Pixel programs by name
const srcCodeExamples: Record<string, string> = {
  // empty example
  '': '',

  fibonacci: `fun fibr(x: int) -> int {
    if (x <= 1) {
        return x;
    }
    return fibr(x-1) + fibr(x-2);
}

fun fib(x: int) -> int {
    let a: int = 0;
    let b: int = 1;
    for (let i: int = 0; i < x; i = i + 1) {
        let tmp: int = b;
        b = a + b;
        a = tmp;
    }
    return a;
}

__pixelr 0, 0, __width, __height, #000000;

__pixelr 0, 0, 1, fibr(1), #00ff00;
__pixelr 1, 0, 1, fibr(2), #00ff00;
__pixelr 2, 0, 1, fibr(3), #00ff00;
__pixelr 3, 0, 1, fibr(4), #00ff00;
__pixelr 4, 0, 1, fibr(5), #00ff00;
__pixelr 5, 0, 1, fibr(6), #00ff00;
__pixelr 6, 0, 1, fibr(7), #00ff00;
__pixelr 7, 0, 1, fibr(8), #00ff00;
__pixelr 8, 0, 1, fibr(9), #00ff00;
__pixelr 9, 0, 1, fibr(10), #00ff00;
__pixelr 10, 0, 1, fibr(11), #00ff00;`,

  race: `/* This function takes two colours (players) and a max score
 * A while loop is used to iteratively draw random numbers for the two
 * players and advance (along the y-axis) the player that gets the
 * highest score. Returns the winner (either 1 or 2) when max score is
 * reached by any of the players
 */
fun Race(p1_c: colour, p2_c : colour, score_max  : int) -> int {
    let p1_score : int = 0;
    let p2_score:int = 0;

    while ((p1_score < score_max) and (p2_score < score_max))
    {
        let p1_toss: int = __randi 10;
        let p2_toss: int = __randi 10;

        if (p1_toss > p2_toss) {
           p1_score = p1_score + 1;
        } else {
          p2_score = p2_score + 1;
        }

        __pixel 1, p1_score, p1_c;
        __pixel 2, p2_score,  p2_c;

        __delay 60;
    }

    let winner: int = 1;
    if (p2_score > p1_score) {
       winner = 2;
    }
    return winner;
}

// Execution (program entry point) starts at the first statement
// that is not a function declaration. This should go in the .main
// function of PixIR.

__pixelr 0, 0, __width, __height, #ffffff;

let c1 : colour = #00ff00; // green
let c2: colour = #0000ff; // blue
let m : int = __height - 1;
let w : int = Race(c1, c2, m);
__print w;`,

  'rule 110': `fun update(row: []bool, n: int) -> []bool {
     let updated: []bool = __newarr bool, n;

     // all array elements must be initialized
     updated[0] = false;
     updated[n-1] = false;
     
     for (let i: int = 1; i < n-1; i = i + 1) {
         let count: int = 0;
         if (row[i-1]) {
            count = count + 1;
         }
         if (row[i]) {
            count = count + 1;
         }
         if (row[i+1]) {
            count = count + 1;
         }

         if (count == 3) {
            updated[i] = false;
         } else {
            if ((count == 2) or row[i] or row[i+1]) {
               updated[i] = true;
            } else {
               updated[i] = false;
            }
         }
     }

     return updated;
}

__pixelr 0, 0, __width, __height, #ffffff;

let row: []bool = __newarr bool, __width;

for (let j: int = 0; j < __width; j = j + 1) {
    row[j] = false;
}

row[4] = true;
row[7] = true;
row[8] = true;
row[10] = true;
row[11] = true;
row[12] = true;
row[13] = true;
row[14] = true;

fun rand_colour() -> colour {
    let x: int = __randi 4;
    if (x == 0) {
        return #FC7300;
    }

    if (x == 1) {
       return #BFDB38;
    }

    if (x == 2) {
        return #1F8A70;
    }

    return #00425A;
}

__pixelr 0, 0, __width, __height, #000000;
for (let i: int = 0; i < __height; i = i + 1) {
    for (let j: int = 0; j < __width; j = j + 1) {
        if (row[j]) {
           __pixel j, i, #000000;
        } else {
           __pixel j, i, rand_colour();
        }
    }
    row = update(row, __width);
    __delay 500;
}`,

  typography: `fun render_letter(x: int, y: int, c: colour, letter: int) -> int {

  // A
  if (letter == 0) {
    __pixel x, y - 1, c;
    __pixel x, y - 2, c;
    __pixel x, y - 3, c;
    __pixel x, y - 4, c;
    __pixel x + 1, y, c;
    __pixel x + 1, y - 2, c;
    __pixel x + 2, y - 1, c;
    __pixel x + 2, y - 2, c;
    __pixel x + 2, y - 3, c;
    __pixel x + 2, y - 4, c;
  }

  // B
  if (letter == 1) {
    __pixel x, y, c;
    __pixel x, y - 1, c;
    __pixel x, y - 2, c;
    __pixel x, y - 3, c;
    __pixel x, y - 4, c;
    __pixel x + 1, y, c;
    __pixel x + 1, y - 2, c;
    __pixel x + 1, y - 4, c;
    __pixel x + 2, y - 1, c;
    __pixel x + 2, y - 2, c;
    __pixel x + 2, y - 3, c;
  }

  // C
  if (letter == 2) {
    __pixel x, y, c;
    __pixel x, y - 1, c;
    __pixel x, y - 2, c;
    __pixel x, y - 3, c;
    __pixel x, y - 4, c;
    __pixel x + 1, y, c;
    __pixel x + 2, y, c;
    __pixel x + 1, y - 4, c;
    __pixel x + 2, y - 4, c;
  }

  // D
  if (letter == 3) {
    __pixel x, y, c;
    __pixel x, y - 1, c;
    __pixel x, y - 2, c;
    __pixel x, y - 3, c;
    __pixel x, y - 4, c;
    __pixel x + 1, y, c;
    __pixel x + 1, y - 4, c;
    __pixel x + 2, y - 1, c;
    __pixel x + 2, y - 2, c;
    __pixel x + 2, y - 3, c;
  }

  // E
  if (letter == 4) {
    __pixel x, y, c;
    __pixel x, y - 1, c;
    __pixel x, y - 2, c;
    __pixel x, y - 3, c;
    __pixel x, y - 4, c;
    __pixel x + 1, y - 2, c;
    __pixel x + 1, y, c;
    __pixel x + 2, y, c;
    __pixel x + 1, y - 4, c;
    __pixel x + 2, y - 4, c;
  }

  // F
  if (letter == 5) {
    __pixel x, y, c;
    __pixel x, y - 1, c;
    __pixel x, y - 2, c;
    __pixel x, y - 3, c;
    __pixel x, y - 4, c;
    __pixel x + 1, y - 2, c;
    __pixel x + 1, y, c;
    __pixel x + 2, y, c;
  }

  // G
  if (letter == 6) {
    __pixel x, y, c;
    __pixel x, y - 1, c;
    __pixel x, y - 2, c;
    __pixel x, y - 3, c;
    __pixel x, y - 4, c;
    __pixel x + 1, y, c;
    __pixel x + 2, y, c;
    __pixel x + 1, y - 4, c;
    __pixel x + 2, y - 2, c;
    __pixel x + 2, y - 3, c;
    __pixel x + 2, y - 4, c;
  }

  // H
  if (letter == 7) {
    __pixel x, y, c;
    __pixel x, y - 1, c;
    __pixel x, y - 2, c;
    __pixel x, y - 3, c;
    __pixel x, y - 4, c;
    __pixel x + 2, y, c;
    __pixel x + 2, y - 1, c;
    __pixel x + 2, y - 2, c;
    __pixel x + 2, y - 3, c;
    __pixel x + 2, y - 4, c;
    __pixel x + 1, y - 2, c;
  }

  // I
  if (letter == 8) {
    __pixel x + 1, y, c;
    __pixel x + 1, y - 1, c;
    __pixel x + 1, y - 2, c;
    __pixel x + 1, y - 3, c;
    __pixel x + 1, y - 4, c;
    __pixel x, y, c;
    __pixel x + 2, y, c;
    __pixel x, y - 4, c;
    __pixel x + 2, y - 4, c;
  }

  // J
  if (letter == 9) {
    __pixel x + 1, y, c;
    __pixel x + 1, y - 1, c;
    __pixel x + 1, y - 2, c;
    __pixel x + 1, y - 3, c;
    __pixel x + 1, y - 4, c;
    __pixel x, y, c;
    __pixel x + 2, y, c;
    __pixel x, y - 4, c;
  }

  // K
  if (letter == 10) {
    __pixel x, y, c;
    __pixel x, y - 1, c;
    __pixel x, y - 2, c;
    __pixel x, y - 3, c;
    __pixel x, y - 4, c;
    __pixel x + 1, y - 2, c;
    __pixel x + 2, y - 1, c;
    __pixel x + 2, y - 3, c;
    __pixel x + 2, y, c;
    __pixel x + 2, y - 4, c;
  }

  // L
  if (letter == 11) {
    __pixel x, y, c;
    __pixel x, y - 1, c;
    __pixel x, y - 2, c;
    __pixel x, y - 3, c;
    __pixel x, y - 4, c;
    __pixel x + 1, y - 4, c;
    __pixel x + 2, y - 4, c;
  }

  // M
  if (letter == 12) {
    __pixel x, y, c;
    __pixel x, y - 1, c;
    __pixel x, y - 2, c;
    __pixel x, y - 3, c;
    __pixel x, y - 4, c;
    __pixel x + 2, y, c;
    __pixel x + 2, y - 1, c;
    __pixel x + 2, y - 2, c;
    __pixel x + 2, y - 3, c;
    __pixel x + 2, y - 4, c;
    __pixel x + 1, y - 1, c;
  }

  // N
  if (letter == 13) {
    __pixel x, y, c;
    __pixel x, y - 1, c;
    __pixel x, y - 2, c;
    __pixel x, y - 3, c;
    __pixel x, y - 4, c;
    __pixel x + 2, y, c;
    __pixel x + 2, y - 1, c;
    __pixel x + 2, y - 2, c;
    __pixel x + 2, y - 3, c;
    __pixel x + 2, y - 4, c;
    __pixel x + 1, y - 1, c;
    __pixel x + 1, y - 2, c;
  }

  // O
  if (letter == 14) {
    __pixel x, y, c;
    __pixel x + 2, y, c;
    __pixel x, y - 4, c;
    __pixel x + 2, y - 4, c;
    __pixel x, y - 1, c;
    __pixel x, y - 2, c;
    __pixel x, y - 3, c;
    __pixel x + 1, y, c;
    __pixel x + 1, y - 4, c;
    __pixel x + 2, y - 1, c;
    __pixel x + 2, y - 2, c;
    __pixel x + 2, y - 3, c;
  }

  // P
  if (letter == 15) {
    __pixel x, y, c;
    __pixel x, y - 1, c;
    __pixel x, y - 2, c;
    __pixel x, y - 3, c;
    __pixel x, y - 4, c;
    __pixel x + 1, y, c;
    __pixel x + 1, y - 2, c;
    __pixel x + 2, y, c;
    __pixel x + 2, y - 1, c;
    __pixel x + 2, y - 2, c;
  }

  // R
  if (letter == 17) {
    __pixel x, y, c;
    __pixel x, y - 1, c;
    __pixel x, y - 2, c;
    __pixel x, y - 3, c;
    __pixel x, y - 4, c;
    __pixel x + 1, y, c;
    __pixel x + 1, y - 2, c;
    __pixel x + 2, y, c;
    __pixel x + 2, y - 1, c;
    __pixel x + 2, y - 2, c;
    __pixel x + 1, y - 3, c;
    __pixel x + 2, y - 4, c;
  }

  // S
  if (letter == 18) {
    __pixel x + 2, y, c;
    __pixel x + 1, y, c;
    __pixel x, y, c;
    __pixel x, y - 1, c;
    __pixel x, y - 2, c;
    __pixel x + 1, y - 2, c;
    __pixel x + 2, y - 2, c;
    __pixel x + 2, y - 3, c;
    __pixel x + 2, y - 4, c;
    __pixel x + 1, y - 4, c;
    __pixel x, y - 4, c;
  }

  // T
  if (letter == 19) {
    __pixel x + 1, y, c;
    __pixel x + 1, y - 1, c;
    __pixel x + 1, y - 2, c;
    __pixel x + 1, y - 3, c;
    __pixel x + 1, y - 4, c;
    __pixel x, y, c;
    __pixel x + 2, y, c;
  }

  // U
  if (letter == 20) {
    __pixel x, y, c;
    __pixel x + 2, y, c;
    __pixel x, y - 4, c;
    __pixel x + 2, y - 4, c;
    __pixel x, y - 1, c;
    __pixel x, y - 2, c;
    __pixel x, y - 3, c;
    __pixel x + 1, y - 4, c;
    __pixel x + 2, y - 1, c;
    __pixel x + 2, y - 2, c;
    __pixel x + 2, y - 3, c;
  }

  // V
  if (letter == 21) {
    __pixel x, y, c;
    __pixel x + 2, y, c;
    __pixel x, y - 1, c;
    __pixel x, y - 2, c;
    __pixel x, y - 3, c;
    __pixel x + 1, y - 4, c;
    __pixel x + 2, y - 1, c;
    __pixel x + 2, y - 2, c;
    __pixel x + 2, y - 3, c;
  }

  // W
  if (letter == 22) {
    __pixel x, y, c;
    __pixel x + 2, y, c;
    __pixel x, y - 1, c;
    __pixel x, y - 2, c;
    __pixel x, y - 3, c;
    __pixel x + 1, y - 4, c;
    __pixel x + 2, y - 1, c;
    __pixel x + 2, y - 2, c;
    __pixel x + 2, y - 3, c;
    __pixel x + 1, y - 3, c;
  }

  // X
  if (letter == 23) {
    __pixel x, y, c;
    __pixel x, y - 1, c;
    __pixel x, y - 3, c;
    __pixel x, y - 4, c;
    __pixel x + 2, y, c;
    __pixel x + 2, y - 1, c;
    __pixel x + 2, y - 3, c;
    __pixel x + 2, y - 4, c;
    __pixel x + 1, y - 2, c;
  }
  // Y
  if (letter == 24) {
    __pixel x, y, c;
    __pixel x, y - 1, c;
    __pixel x, y - 2, c;
    __pixel x + 1, y - 2, c;
    __pixel x + 2, y, c;
    __pixel x + 2, y - 1, c;
    __pixel x + 2, y - 2, c;
    __pixel x + 2, y - 3, c;
    __pixel x + 2, y - 4, c;
    __pixel x + 1, y - 2, c;
    __pixel x + 1, y - 4, c;
    __pixel x, y - 4, c;
  }
  // Z
  if (letter == 25) {
    __pixel x, y, c;
    __pixel x + 1, y, c;
    __pixel x + 2, y, c;
    __pixel x + 2, y - 1, c;
    __pixel x + 1, y - 2, c;
    __pixel x, y - 3, c;
    __pixel x, y - 4, c;
    __pixel x + 1, y - 4, c;
    __pixel x + 2, y - 4, c;
  }

  // .
  if (letter == 26) {
    __pixel x + 1, y - 4, c;
  }

  // ,
  if (letter == 27) {
    __pixel x + 1, y - 3, c;
    __pixel x + 1, y - 4, c;
    __pixel x, y - 4, c;
  }

  // space
  if (letter == 28) {
  }

  return 0;
}

fun render_text(x: int, y: int, c: colour, letters: []int, len: int) -> int {
    let res: int = 0;

    for (let i: int = 0; i < len; i = i + 1) {
        res = render_letter(x, y, c, letters[i]);
        x = x + 4;
        if (x + 3 >= __width) {
            y = y - 6;
            x = 1;
        }
    }

    return 0;
}

let len: int = 140;
let letters: []int = __newarr int, len;

letters[0] = 19;   // T
letters[1] = 7;    // H
letters[2] = 4;    // E
letters[3] = 28;   // 
letters[4] = 15;   // P
letters[5] = 8;    // I
letters[6] = 23;   // X
letters[7] = 4;    // E
letters[8] = 11;   // L
letters[9] = 28;   // 
letters[10] = 11;  // L
letters[11] = 0;   // A
letters[12] = 13;  // N
letters[13] = 6;   // G
letters[14] = 20;  // U
letters[15] = 0;   // A
letters[16] = 6;   // G
letters[17] = 4;   // E
letters[18] = 28;  // 
letters[19] = 8;   // I
letters[20] = 18;  // S
letters[21] = 28;  // 
letters[22] = 0;   // A
letters[23] = 28;  // 
letters[24] = 7;   // H
letters[25] = 8;   // I
letters[26] = 6;   // G
letters[27] = 7;   // H
letters[28] = 28;  // 
letters[29] = 11;  // L
letters[30] = 4;   // E
letters[31] = 21;  // V
letters[32] = 4;   // E
letters[33] = 11;  // L
letters[34] = 28;  // 
letters[35] = 11;  // L
letters[36] = 0;   // A
letters[37] = 13;  // N
letters[38] = 6;   // G
letters[39] = 20;  // U
letters[40] = 0;   // A
letters[41] = 6;   // G
letters[42] = 4;   // E
letters[43] = 28;  // 
letters[44] = 28;  // 
letters[45] = 28;  // 
letters[46] = 28;  // 
letters[47] = 28;  // 
letters[48] = 19;  // T
letters[49] = 0;   // A
letters[50] = 17;  // R
letters[51] = 6;   // G
letters[52] = 4;   // E
letters[53] = 19;  // T
letters[54] = 8;   // I
letters[55] = 13;  // N
letters[56] = 6;   // G
letters[57] = 28;  // 
letters[58] = 19;  // T
letters[59] = 7;   // H
letters[60] = 4;   // E
letters[61] = 28;  // 
letters[62] = 15;  // P
letters[63] = 8;   // I
letters[64] = 23;  // X
letters[65] = 4;   // E
letters[66] = 11;  // L
letters[67] = 28;  // 
letters[68] = 21;  // V
letters[69] = 12;  // M
letters[70] = 27;  // ,
letters[71] = 28;  // 
letters[72] = 0;   // A
letters[73] = 28;  // 
letters[74] = 11;  // L
letters[75] = 14;  // O
letters[76] = 22;  // W
letters[77] = 28;  //
letters[78] = 11;  // L
letters[79] = 4;   // E
letters[80] = 21;  // V
letters[81] = 4;   // E
letters[82] = 11;  // L
letters[83] = 28;  // 
letters[84] = 18;  // S
letters[85] = 19;  // T
letters[86] = 0;   // A
letters[87] = 2;   // C
letters[88] = 10;  // K
letters[89] = 28;  // 
letters[90] = 1;   // B
letters[91] = 0;   // A
letters[92] = 18;  // S
letters[93] = 4;   // E
letters[94] = 3;   // D
letters[95] = 28;  // 
letters[96] = 21;  // V
letters[97] = 8;   // I
letters[98] = 17;  // R
letters[99] = 19;  // T
letters[100] = 20; // U
letters[101] = 0;  // A
letters[102] = 11; // L
letters[103] = 28; // 
letters[104] = 12; // M
letters[105] = 0;  // A
letters[106] = 2;  // C
letters[107] = 7;  // H
letters[108] = 8;  // I
letters[109] = 13; // N
letters[110] = 4;  // E
letters[111] = 28; // 
letters[112] = 22; // W
letters[113] = 8;  // I
letters[114] = 19; // T
letters[115] = 7;  // H
letters[116] = 28; //
letters[117] = 0;  // A
letters[118] = 28; // 
letters[119] = 28; // 
letters[120] = 6;  // G
letters[121] = 17; // R
letters[122] = 0;  // A
letters[123] = 15; // P
letters[124] = 7;  // H
letters[125] = 8;  // I
letters[126] = 2;  // C
letters[127] = 0;  // A
letters[128] = 11; // L
letters[129] = 28; // 
letters[130] = 8;  // I
letters[131] = 13; // N
letters[132] = 19; // T
letters[133] = 4;  // E
letters[134] = 17; // R
letters[135] = 5;  // F
letters[136] = 0;  // A
letters[137] = 2;  // C
letters[138] = 4;  // E
letters[139] = 26; // .

let x: int = 1;
let y: int = __height - 1;

__pixelr 0, 0, __width, __height, #000000;
let res: int = render_text(x, y, #ff0000, letters, len);

len = 138;
letters = __newarr int, len;

x = 1;
y = __height - 40;
letters[0] = 19;   // T
letters[1] = 7;    // H
letters[2] = 4;    // E
letters[3] = 28;   // 
letters[4] = 21;   // V
letters[5] = 12;   // M
letters[6] = 28;   // 
letters[7] = 8;    // I
letters[8] = 13;   // N
letters[9] = 18;   // S
letters[10] = 19;  // T
letters[11] = 17;  // R
letters[12] = 20;  // U
letters[13] = 2;   // C
letters[14] = 19;  // T
letters[15] = 8;   // I
letters[16] = 14;  // O
letters[17] = 13;  // N
letters[18] = 28;  // 
letters[19] = 18;  // S
letters[20] = 4;   // E
letters[21] = 19;  // T
letters[22] = 28;  // 
letters[23] = 28;  // 
letters[24] = 8;   // I
letters[25] = 13;  // N
letters[26] = 2;   // C
letters[27] = 11;  // L
letters[28] = 20;  // U
letters[29] = 3;   // D
letters[30] = 4;   // E
letters[31] = 18;  // S
letters[32] = 28;  // 
letters[33] = 8;   // I
letters[34] = 13;  // N
letters[35] = 18;  // S
letters[36] = 19;  // T
letters[37] = 17;  // R
letters[38] = 20;  // U
letters[39] = 2;   // C
letters[40] = 19;  // T
letters[41] = 8;   // I
letters[42] = 14;  // O
letters[43] = 13;  // N
letters[44] = 18;  // S
letters[45] = 28;  // 
letters[46] = 28;  // 
letters[47] = 28;  // 
letters[48] = 5;   // F
letters[49] = 14;  // O
letters[50] = 17;  // R
letters[51] = 28;  // 
letters[52] = 2;   // C
letters[53] = 11;  // L
letters[54] = 4;   // E
letters[55] = 0;   // A
letters[56] = 17;  // R
letters[57] = 8;   // I
letters[58] = 13;  // N
letters[59] = 6;   // G
letters[60] = 28;  // 
letters[61] = 19;  // T
letters[62] = 7;   // H
letters[63] = 4;   // E
letters[64] = 28;  // 
letters[65] = 3;   // D
letters[66] = 8;   // I
letters[67] = 18;  // S
letters[68] = 15;  // P
letters[69] = 11;  // L
letters[70] = 0;   // A
letters[71] = 24;  // Y
letters[72] = 27;  // ,
letters[73] = 28;  // 
letters[74] = 0;   // A
letters[75] = 18;  // S
letters[76] = 28;  // 
letters[77] = 22;  // W
letters[78] = 4;   // E
letters[79] = 11;  // L
letters[80] = 11;  // L
letters[81] = 28;  // 
letters[82] = 0;   // A
letters[83] = 18;  // S
letters[84] = 28;  // 
letters[85] = 18;  // S
letters[86] = 4;   // E
letters[87] = 19;  // T
letters[88] = 19;  // T
letters[89] = 8;   // I
letters[90] = 13;  // N
letters[91] = 6;   // G
letters[92] = 28;  //
letters[93] = 28;  // 
letters[94] = 28;  // 
letters[95] = 28;  // 
letters[96] = 18;  // S
letters[97] = 15;  // P
letters[98] = 4;   // E
letters[99] = 2;   // C
letters[100] = 8;  // I
letters[101] = 5;  // F
letters[102] = 8;  // I
letters[103] = 2;  // C
letters[104] = 28; // 
letters[105] = 15; // P
letters[106] = 8;  // I
letters[107] = 23; // X
letters[108] = 4;  // E
letters[109] = 11; // L
letters[110] = 18; // S
letters[111] = 28; // 
letters[112] = 14; // O
letters[113] = 17; // R
letters[114] = 28; // 
letters[115] = 28; // 
letters[116] = 28; // 
letters[117] = 28; // 
letters[118] = 28; // 
letters[119] = 28; // 
letters[120] = 17; // R
letters[121] = 4;  // E
letters[122] = 6;  // G
letters[123] = 8;  // I
letters[124] = 14; // O
letters[125] = 13; // N
letters[126] = 18; // S
letters[127] = 28; // 
letters[128] = 22; // W
letters[129] = 8;  // I
letters[130] = 19; // T
letters[131] = 7;  // H
letters[132] = 8;  // I
letters[133] = 13; // N
letters[134] = 28; //
letters[135] = 8;  // I
letters[136] = 19; // T
letters[137] = 26; // .

res = render_text(x, y, #0070ff, letters, len);`,

  'game of life': `/* Demonstration of Gosper glider gun.
 * SET SCREEN DIMENSIONS TO 40 x 30 FOR BEST RESULTS.
 * Larger screen dimensions will lag a lot.
 */
fun min(x: int, y: int) -> int {
    if (x < y) {
       return x;
    }
    return y;
}

fun max(x: int, y: int) -> int {
    if (x > y) {
       return x;
    }
    return y;
}

fun allocGrid(w: int, h: int) -> [][]colour {
    let grid: [][]colour = __newarr []colour, w;

    for (let x: int = 0; x < w; x = x + 1) {
        grid[x] = __newarr colour, h;
        for (let y: int = 0; y < h; y = y + 1) {
            grid[x][y] = #ffffff;
        }
    }

    return grid;
}

fun drawGrid(grid: [][]colour, w: int, h: int) -> int {
   for (let x: int = 0; x < w; x = x + 1) {
      for (let y: int = 0; y < h; y = y + 1) {
         __pixel x, y, grid[x][y];
      }
   }

   return 0;
}

fun alive(grid: [][]colour, x: int, y: int) -> bool {
    return grid[x][y] == #000000;
}

fun countNeighbours(grid: [][]colour, x: int, y: int, w: int, h: int) -> int {
    let sum: int = 0;

    for (let i: int = max(x-1, 0); i <= min(x+1, w-1); i = i + 1) {
        for (let j: int = max(y-1, 0); j <= min(y+1, h-1); j = j + 1) {
            if (alive(grid, i, j) and ((i != x) or (j != y))) {
               sum = sum + 1;
            }
        }
    }

    return sum;
}

fun gameOfLifeIter(grid: [][]colour, swapbuf: [][]colour, w: int, h: int) -> int {
    for (let x: int = 0; x < w; x = x + 1) {
        for (let y: int = 0; y < h; y = y + 1) {
            let neighbours: int = countNeighbours(grid, x, y, w, h);

            swapbuf[x][y] = grid[x][y];

            if (alive(grid, x, y)) {
               if ((neighbours < 2) or (neighbours > 3)) {
                  swapbuf[x][y] = #ffffff;
               }
            } else {
              if (neighbours == 3) {
                 swapbuf[x][y] = #000000;
              }
            }
        }
    }

    return 0;
}

let width: int = __width;
let height: int = __height;
let grid: [][]colour = allocGrid(width, height);
let swapbuf: [][]colour = allocGrid(width, height);

let steps: int = 2000;
let delay: int = 100;

// initial setup for Gosper glider gun
grid[1][14] = #000000;
grid[1][15] = #000000;
grid[2][14] = #000000;
grid[2][15] = #000000;

grid[11][13] = #000000;
grid[11][14] = #000000;
grid[11][15] = #000000;

grid[12][12] = #000000;
grid[12][16] = #000000;

grid[13][11] = #000000;
grid[13][17] = #000000;

grid[14][11] = #000000;
grid[14][17] = #000000;

grid[15][14] = #000000;

grid[16][12] = #000000;
grid[16][16] = #000000;

grid[17][13] = #000000;
grid[17][14] = #000000;
grid[17][15] = #000000;

grid[18][14] = #000000;

grid[21][15] = #000000;
grid[21][16] = #000000;
grid[21][17] = #000000;

grid[22][15] = #000000;
grid[22][16] = #000000;
grid[22][17] = #000000;

grid[23][14] = #000000;
grid[23][18] = #000000;

grid[25][13] = #000000;
grid[25][14] = #000000;
grid[25][18] = #000000;
grid[25][19] = #000000;

grid[35][16] = #000000;
grid[35][17] = #000000;
grid[36][16] = #000000;
grid[36][17] = #000000;

// draw grid initially
let v_: int = drawGrid(grid, width, height);

for (let step: int = 0; step < steps; step = step + 1) {
    // compute one iteration of the game of life into swapbuf
    v_ = gameOfLifeIter(grid, swapbuf, width, height);

    __print step;

    // swap grid and swapbuf
    let tmp: [][]colour = swapbuf;
    swapbuf = grid;
    grid = tmp;

    // draw updated grid
    v_ = drawGrid(grid, width, height);

    __delay delay;
}`,

  'wall clock': `// perform integer division using Euclids algorithm.
fun idiv(n: int, d: int) -> int {
    // hack to handle division when n is negative.
    let negative: bool = n < 0;
    if (negative) {
       n = -n;
    }

    let r: int = n;
    let q: int = 0;

    while (r >= d) {
        r = r - d;
        q = q + 1;
    }

    if (negative) {
       return -q;
    }

    return q;
}

// perform integer modulo using Euclids algorithm.
fun imod(n: int, d: int) -> int {
    let r: int = n;
    let q: int = 0;

    while (r >= d) {
        r = r - d;
        q = q + 1;
    }

    return r;
}

fun min(x: int, y: int) -> int {
    if (x <= y) {
       return x;
    }
    return y;
}

fun rsin_theta(r: int, theta: int) -> int {
    let sin_table: []int = __newarr int, 60;
    let amplitude_factor: int = 512;

    sin_table[0] = 0;
    sin_table[1] = 53;
    sin_table[2] = 106;
    sin_table[3] = 158;
    sin_table[4] = 208;
    sin_table[5] = 255;
    sin_table[6] = 300;
    sin_table[7] = 342;
    sin_table[8] = 380;
    sin_table[9] = 414;
    sin_table[10] = 443;
    sin_table[11] = 467;
    sin_table[12] = 486;
    sin_table[13] = 500;
    sin_table[14] = 509;
    sin_table[15] = 512;
    sin_table[16] = 509;
    sin_table[17] = 500;
    sin_table[18] = 486;
    sin_table[19] = 467;
    sin_table[20] = 443;
    sin_table[21] = 414;
    sin_table[22] = 380;
    sin_table[23] = 342;
    sin_table[24] = 300;
    sin_table[25] = 255;
    sin_table[26] = 208;
    sin_table[27] = 158;
    sin_table[28] = 106;
    sin_table[29] = 53;
    sin_table[30] = 0;
    sin_table[31] = -53;
    sin_table[32] = -106;
    sin_table[33] = -158;
    sin_table[34] = -208;
    sin_table[35] = -255;
    sin_table[36] = -300;
    sin_table[37] = -342;
    sin_table[38] = -380;
    sin_table[39] = -414;
    sin_table[40] = -443;
    sin_table[41] = -467;
    sin_table[42] = -486;
    sin_table[43] = -500;
    sin_table[44] = -509;
    sin_table[45] = -512;
    sin_table[46] = -509;
    sin_table[47] = -500;
    sin_table[48] = -486;
    sin_table[49] = -467;
    sin_table[50] = -443;
    sin_table[51] = -414;
    sin_table[52] = -380;
    sin_table[53] = -342;
    sin_table[54] = -300;
    sin_table[55] = -255;
    sin_table[56] = -208;
    sin_table[57] = -158;
    sin_table[58] = -106;
    sin_table[59] = -53;

   return idiv(r * sin_table[idiv(theta, 6)], amplitude_factor);
}

fun rcos_theta(r: int, theta: int) -> int {
    let cos_table: []int = __newarr int, 60;
    let amplitude_factor: int = 512;

    cos_table[0] = 512;
    cos_table[1] = 509;
    cos_table[2] = 500;
    cos_table[3] = 486;
    cos_table[4] = 467;
    cos_table[5] = 443;
    cos_table[6] = 414;
    cos_table[7] = 380;
    cos_table[8] = 342;
    cos_table[9] = 300;
    cos_table[10] = 256;
    cos_table[11] = 208;
    cos_table[12] = 158;
    cos_table[13] = 106;
    cos_table[14] = 53;
    cos_table[15] = 0;
    cos_table[16] = -53;
    cos_table[17] = -106;
    cos_table[18] = -158;
    cos_table[19] = -208;
    cos_table[20] = -255;
    cos_table[21] = -300;
    cos_table[22] = -342;
    cos_table[23] = -380;
    cos_table[24] = -414;
    cos_table[25] = -443;
    cos_table[26] = -467;
    cos_table[27] = -486;
    cos_table[28] = -500;
    cos_table[29] = -509;
    cos_table[30] = -512;
    cos_table[31] = -509;
    cos_table[32] = -500;
    cos_table[33] = -486;
    cos_table[34] = -467;
    cos_table[35] = -443;
    cos_table[36] = -414;
    cos_table[37] = -380;
    cos_table[38] = -342;
    cos_table[39] = -300;
    cos_table[40] = -256;
    cos_table[41] = -208;
    cos_table[42] = -158;
    cos_table[43] = -106;
    cos_table[44] = -53;
    cos_table[45] = 0;
    cos_table[46] = 53;
    cos_table[47] = 106;
    cos_table[48] = 158;
    cos_table[49] = 208;
    cos_table[50] = 256;
    cos_table[51] = 300;
    cos_table[52] = 342;
    cos_table[53] = 380;
    cos_table[54] = 414;
    cos_table[55] = 443;
    cos_table[56] = 467;
    cos_table[57] = 486;
    cos_table[58] = 500;
    cos_table[59] = 509;

   return idiv(r * cos_table[idiv(theta, 6)], amplitude_factor);
}

// radius of the clock face
let clock_radius: int = idiv(min(__width, __height), 2) - 2;

let clock_origin_x: int = idiv(__width, 2);
let clock_origin_y: int = idiv(__height, 2);

// clear the display
__pixelr 0, 0, __width, __height, #000000;

// draw clock face

__pixel clock_origin_x + rcos_theta(clock_radius, 0), clock_origin_y + rsin_theta(clock_radius, 0), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-1, 0), clock_origin_y + rsin_theta(clock_radius-1, 0), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-2, 0), clock_origin_y + rsin_theta(clock_radius-2, 0), #D0D0D0;

__pixel clock_origin_x + rcos_theta(clock_radius, 30), clock_origin_y + rsin_theta(clock_radius, 30), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-1, 30), clock_origin_y + rsin_theta(clock_radius-1, 30), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-2, 30), clock_origin_y + rsin_theta(clock_radius-2, 30), #D0D0D0;

__pixel clock_origin_x + rcos_theta(clock_radius, 60), clock_origin_y + rsin_theta(clock_radius, 60), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-1, 60), clock_origin_y + rsin_theta(clock_radius-1, 60), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-2, 60), clock_origin_y + rsin_theta(clock_radius-2, 60), #D0D0D0;

__pixel clock_origin_x + rcos_theta(clock_radius, 90), clock_origin_y + rsin_theta(clock_radius, 90), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-1, 90), clock_origin_y + rsin_theta(clock_radius-1, 90), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-2, 90), clock_origin_y + rsin_theta(clock_radius-2, 90), #D0D0D0;

__pixel clock_origin_x + rcos_theta(clock_radius, 120), clock_origin_y + rsin_theta(clock_radius, 120), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-1, 120), clock_origin_y + rsin_theta(clock_radius-1, 120), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-2, 120), clock_origin_y + rsin_theta(clock_radius-2, 120), #D0D0D0;

__pixel clock_origin_x + rcos_theta(clock_radius, 150), clock_origin_y + rsin_theta(clock_radius, 150), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-1, 150), clock_origin_y + rsin_theta(clock_radius-1, 150), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-2, 150), clock_origin_y + rsin_theta(clock_radius-2, 150), #D0D0D0;

__pixel clock_origin_x + rcos_theta(clock_radius, 180), clock_origin_y + rsin_theta(clock_radius, 180), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-1, 180), clock_origin_y + rsin_theta(clock_radius-1, 180), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-2, 180), clock_origin_y + rsin_theta(clock_radius-2, 180), #D0D0D0;

__pixel clock_origin_x + rcos_theta(clock_radius, 210), clock_origin_y + rsin_theta(clock_radius, 210), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-1, 210), clock_origin_y + rsin_theta(clock_radius-1, 210), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-2, 210), clock_origin_y + rsin_theta(clock_radius-2, 210), #D0D0D0;

__pixel clock_origin_x + rcos_theta(clock_radius, 240), clock_origin_y + rsin_theta(clock_radius, 240), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-1, 240), clock_origin_y + rsin_theta(clock_radius-1, 240), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-2, 240), clock_origin_y + rsin_theta(clock_radius-2, 240), #D0D0D0;

__pixel clock_origin_x + rcos_theta(clock_radius, 270), clock_origin_y + rsin_theta(clock_radius, 270), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-1, 270), clock_origin_y + rsin_theta(clock_radius-1, 270), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-2, 270), clock_origin_y + rsin_theta(clock_radius-2, 270), #D0D0D0;

__pixel clock_origin_x + rcos_theta(clock_radius, 300), clock_origin_y + rsin_theta(clock_radius, 300), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-1, 300), clock_origin_y + rsin_theta(clock_radius-1, 300), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-2, 300), clock_origin_y + rsin_theta(clock_radius-2, 300), #D0D0D0;

__pixel clock_origin_x + rcos_theta(clock_radius, 330), clock_origin_y + rsin_theta(clock_radius, 330), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-1, 330), clock_origin_y + rsin_theta(clock_radius-1, 330), #D0D0D0;
__pixel clock_origin_x + rcos_theta(clock_radius-2, 330), clock_origin_y + rsin_theta(clock_radius-2, 330), #D0D0D0;

// length of clock hand
let hand_length: int = clock_radius - 5;

let clock_pos: int = 12;
let ticks: int = 300;


for (let i: int = 0; i < ticks; i = i + 1) {
    // render clock hand
    for (let r: int = 1; r < hand_length; r = r + 1) {
        __pixel clock_origin_x - rcos_theta(imod(r, 60), 6*clock_pos), clock_origin_y + rsin_theta(imod(r, 60), 6*clock_pos), #ff0000;
    }

    __delay 600;

    // erase clock hand
    for (let r: int = 1; r < hand_length; r = r + 1) {
        __pixel clock_origin_x - rcos_theta(imod(r, 60), 6*clock_pos), clock_origin_y + rsin_theta(imod(r, 60), 6*clock_pos), #000000;
    }

    clock_pos = imod(clock_pos + 1, 60);
}`,

  clear: `// clear screen to a specific colour
__pixelr 0, 0, __width, __height, #000000;`
}

const setCodeToExample = (exampleName: string) => {
  ;(srcCodeEditor.value as typeof CodeEditor).setCode(srcCodeExamples[exampleName])
}
</script>
