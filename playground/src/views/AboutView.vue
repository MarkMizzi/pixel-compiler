<template>
  <div class="about">
    <h1>Introduction</h1>

    <p>
      This webpage is a playground for the Pixel Language. The compiler was initially developed as
      part of an assignment for a
      <a class="link-green" href="https://www.um.edu.mt/courses/studyunit/CPS2000"
        >course on Compiler Design</a
      >
      while I was studying for a Bachelors in Computer Science.
    </p>

    <p>
      The Pixel language is Turing complete, imperative and has builtins for interacting with a
      graphical display. This allows the Pixel programmer to easily write programs that produce
      interesting visuals and also makes Pixel appealing as a toy language.
    </p>

    <p>A full specification of the Pixel language is given below.</p>

    <p>
      Pixel is intended to be compiled down to an intermediate representation (IR) for a stack-based
      virtual machine (VM) referred to as the Pixel VM. The VM state includes a graphical display
      and opcodes are provided for manipulating this display.
    </p>

    <p>A full specification of the Pixel VM is given below.</p>

    <h1>Pixel Language Specification</h1>

    <h2>Syntax</h2>

    <p>
      The syntax of the Pixel language is specified entirely by the following extended BNF grammar:
    </p>

    <code class="font-mono">
      &lt; Letter &gt; ::= [A-Za-z] <br />
      &lt; Digit &gt; ::= [0-9] <br />
      &lt; Hex &gt; ::= [A-Fa-f] | &lt; Digit &gt; <br /><br />

      &lt; Type &gt; ::= &quot;float&quot; | &quot;int&quot; | &quot;bool&quot; | &quot;colour&quot;
      | &quot;[&quot; &quot;]&quot; &lt; Type &gt; <br /><br />

      &lt; BooleanLiteral&gt; ::= &quot;true&quot; | &quot;false&quot; <br />
      &lt; IntegerLiteral&gt;::= &lt; Digit &gt; { &lt; Digit &gt; } <br />
      &lt; FloatLiteral&gt; ::= &lt; Digit &gt; { &lt; Digit &gt; } &quot;.&quot; &lt; Digit &gt; {
      &lt; Digit &gt; } <br />
      &lt; ColourLiteral&gt; ::= &quot;#&quot; &lt; Hex &gt; &lt; Hex &gt; &lt; Hex &gt; &lt; Hex
      &gt; &lt; Hex &gt; &lt; Hex &gt; <br />

      &lt;Literal&gt; ::= <br />
      &emsp;&lt; BooleanLiteral &gt; | <br />
      &emsp;&lt; IntegerLiteral &gt; | <br />
      &emsp;&lt; FloatLiteral &gt; | <br />
      &emsp;&lt; ColourLiteral &gt; <br /><br />

      &lt;Identifier &gt; ::= ( &lt; Letter &gt; ) { &quot;_&quot; | &lt;Letter &gt; | &lt;Digit&gt;
      } <br /><br />

      &lt;MultiplicativeOp&gt; ::= &quot;*&quot; | &quot;/&quot; | &quot;and&quot; <br />
      &lt;AdditiveOp&gt; ::= &quot;+&quot; | &quot;-&quot; | &quot;or&quot; <br />
      &lt;RelationalOp&gt; ::= &quot;&lt;&quot; | &quot;&gt;&quot; | &quot;==&quot; | &quot;!=&quot;
      | &quot;&lt;=&quot; | &quot;&gt;=&quot; <br /><br />

      &lt; ActualParams &gt; ::= &lt; Expr &gt; { &quot;,&quot; &lt; Expr &gt; } <br />
      &lt; FunctionCall &gt; ::= &lt; Identifier &gt; &quot;(&quot; [ &lt; ActualParams &gt; ]
      &quot;)&quot; <br /><br />

      &lt; ArrayAccessExpr &gt; ::= ( &lt; Identifier &gt; | &lt; ArrayAccessExpr &gt; )
      &quot;[&quot; &lt; Expr &gt; &quot;]&quot; <br />
      &lt; NewArrExpr &gt; ::= &quot;__newarr&quot; &lt; Type &gt; &quot;,&quot; &lt; Expr &gt;
      <br />
      &lt; SubExpr &gt; ::= &quot;(&quot; &lt; Expr &gt; &quot;)&quot; <br />
      &lt; Unary &gt; ::= ( &quot;-&quot; | &quot;not&quot; ) &lt; Expr &gt; <br />
      &lt; Factor &gt; ::= <br />
      &emsp;&lt; Literal &gt; | <br />
      &emsp;&lt; Identifier &gt; | <br />
      &emsp;&lt; FunctionCall &gt; | <br />
      &emsp;&lt; SubExpr &gt; | <br />
      &emsp;&lt; Unary &gt; | <br />
      &emsp;&quot;__randi&quot; &lt; Expr &gt; | <br />
      &emsp;&quot;__width&quot; | <br />
      &emsp;&quot;__height&quot; | <br />
      &emsp;&quot;__read&quot; &lt; Expr &gt; &quot;,&quot; &lt; Expr &gt; | <br />
      &emsp;&lt; ArrayAccessExpr &gt; | <br />
      &emsp;&lt; NewArrExpr &gt; <br /><br />

      &lt; Term &gt; ::= &lt; Factor &gt; { &lt; MultiplicativeOp &gt; &lt; Factor &gt; } <br />
      &lt; SimpleExpr &gt; ::= &lt; Term &gt; { &lt; AdditiveOp &gt; &lt; Term &gt; } <br />
      &lt; Expr &gt; ::= &lt; SimpleExpr &gt; { &lt; RelationalOp &gt; &lt; SimpleExpr &gt; }
      <br /><br />

      &lt; LValue &gt; ::= &lt; ArrayAccessExpr &gt; | &lt; Identifier &gt; <br />
      &lt; Assignment &gt; ::= &lt; LValue &gt; &quot;=&quot; &lt; Expr &gt; <br /><br />

      &lt; VariableDecl &gt; ::= &quot;let&quot; &lt; Identifier &gt; &quot;:&quot; &lt; Type &gt;
      &quot;=&quot; &lt; Expr &gt; <br /><br />

      &lt; PrintStatement &gt; ::= &quot;__print&quot; &lt; Expr &gt; <br />
      &lt; DelayStatement &gt; ::= &quot;__delay&quot; &lt; Expr &gt; <br />
      &lt; PixelStatement &gt; ::= <br />
      &emsp;&quot;__pixelr&quot; &lt; Expr &gt; &quot;,&quot; &lt; Expr &gt; &quot;,&quot; &lt; Expr
      &gt; &quot;,&quot;&lt; Expr &gt; &quot;,&quot; &lt;Expr &gt; | <br />
      &emsp;&quot;__pixel&quot; &lt; Expr &gt; &quot;,&quot; &lt; Expr &gt; &quot;,&quot; &lt; Expr
      &gt; <br />
      &lt; RtrnStatement &gt; ::= &quot;return&quot; &lt; Expr &gt; <br /><br />

      &lt; IfStatement &gt; ::= <br />
      &emsp;&quot;if&quot; &quot;(&quot; &lt; Expr &gt; &quot;)&quot; &lt; Block &gt; <br />
      &emsp;&emsp;[ &quot;else&quot; &lt; Block &gt; ] <br /><br />

      &lt; ForStatement &gt; ::= <br />
      &emsp;&quot;for&quot; &quot;(&quot; [ &lt; VariableDecl &gt; ] &quot;;&quot; &lt; Expr &gt;
      &quot;;&quot; [ &lt; Assignment &gt; ] &quot;)&quot; <br />
      &emsp;&emsp;&lt; Block &gt; <br /><br />

      &lt; WhileStatement &gt; ::= &quot;while&quot; &quot;(&quot; &lt;Expr &gt; &quot;)&quot; &lt;
      Block &gt; <br /><br />

      &lt; FormalParam &gt; ::= &lt; Identifier &gt; &quot;:&quot; &lt; Type &gt; <br />
      &lt; FormalParams&gt; ::= &lt; FormalParam &gt; { &quot;,&quot; &lt; FormalParam &gt; } <br />
      &lt; FunctionDecl &gt; ::= <br />
      &emsp;&quot;fun&quot; &lt; Identifier &gt; &quot;(&quot; [ &lt;FormalParams&gt; ]
      &quot;)&quot; <br />
      &emsp;&emsp;&quot;->&quot; &lt; Type &gt; &lt; Block &gt; <br /><br />

      &lt; Statement &gt; ::= <br />
      &emsp;&lt; VariableDecl &gt; &quot;;&quot; | <br />
      &emsp;&lt; Assignment &gt; &quot;;&quot; | <br />
      &emsp;&lt; PrintStatement &gt; &quot;;&quot; | <br />
      &emsp;&lt; DelayStatement &gt; &quot;;&quot; | <br />
      &emsp;&lt; PixelStatement &gt; &quot;;&quot; | <br />
      &emsp;&lt; IfStatement &gt; | <br />
      &emsp;&lt; ForStatement &gt; | <br />
      &emsp;&lt; WhileStatement &gt; | <br />
      &emsp;&lt; RtrnStatement &gt; &quot;;&quot; | <br />
      &emsp;&lt; FunctionDecl &gt; | <br />
      &emsp;&lt; Block &gt; <br /><br />

      &lt; Block &gt; ::= &quot;{&quot; { &lt; Statement &gt; } &quot;}&quot; <br />
      &lt; Program &gt; ::= { &lt; Statement &gt; } <br /><br />
    </code>
    <!-- BNF -->

    <h2>Semantics</h2>

    <p>
      Many constructs in Pixel are similar to C or other imperative languages, and their semantics
      can therefore be inferred readily. This section is therefore not an attempt to accurately and
      completely describe all the semantics of Pixel, but instead focuses on an informal description
      of the more idiosyncratic of Pixel's semantics.
    </p>

    <p>
      Code can be written outside of any function. Such code will be placed inside a special
      <span class="code">main</span> function by the compiler which is the designated entry point of
      a Pixel program. No function defined by the programmer can be called
      <span class="code">main</span>.
    </p>

    <p>
      Function definitions can be nested. This is mainly an organizational tool, as functions are
      not treated as first class objects in Pixel, and cannot be passed as arguments or returned
      from a function.
    </p>

    <p>
      Pixel contains four simple types: <span class="code">float</span>,
      <span class="code">int</span>, <span class="code">bool</span> and
      <span class="code">colour</span>. <span class="code">colour</span> can be used in various
      graphical operations on the display that is part of the Pixel programming environment. Pixel
      also supports arrays of any type (including arrays of arrays).
    </p>

    <p>There is no type casting in Pixel.</p>

    <p>
      Arithmetic and logical operators have the expected input and output types, with the exception
      of division when applied to two integers, which returns a float rather than another integer.
    </p>

    <p>
      Pixel comes with a number of &quot;builtins&quot; which are invoked like functions (except
      there are no parantheses around the arguments). These are listed in the following table:
    </p>

    <table>
      <thead>
        <tr>
          <th>Builtin</th>
          <th>Expected Input Types</th>
          <th>Output Type</th>
          <th>Description</th>
        </tr>
      </thead>
      <tbody>
        <tr>
          <td><span class="code">__width</span></td>
          <td>N/A</td>
          <td><span class="code">int</span></td>
          <td>Returns the width of the Pixel display.</td>
        </tr>
        <tr>
          <td><span class="code">__height</span></td>
          <td>N/A</td>
          <td><span class="code">int</span></td>
          <td>Returns the height of the Pixel display.</td>
        </tr>
        <tr>
          <td><span class="code">__read x, y</span></td>
          <td><span class="code">int, int</span></td>
          <td><span class="code">colour</span></td>
          <td>
            Returns the colour of the pixel at
            <span class="code">x, y</span>
            on the Pixel display.
          </td>
        </tr>
        <tr>
          <td><span class="code">__randi x</span></td>
          <td><span class="code">int</span></td>
          <td><span class="code">int</span></td>
          <td>
            Returns a random integer in the range
            <span class="code">[0, x)</span>.
          </td>
        </tr>
        <tr>
          <td><span class="code">__print x</span></td>
          <td>Any type</td>
          <td>N/A</td>
          <td>
            Prints
            <span class="code">x</span>
            to Pixel's output log.
          </td>
        </tr>
        <tr>
          <td><span class="code">__delay x</span></td>
          <td><span class="code">int</span></td>
          <td>N/A</td>
          <td>
            Delay execution of the next program statement by
            <span class="code">x</span>
            milliseconds.
          </td>
        </tr>
        <tr>
          <td><span class="code">__pixelr x, y, w, h, c</span></td>
          <td><span class="code">int, int, int, int, colour</span></td>
          <td>N/A</td>
          <td>
            Set the pixels within the rectange of width
            <span class="code">w</span>
            and height
            <span class="code">h</span>
            with top-left corner at
            <span class="code">x, y</span>
            to the colour
            <span class="code">c</span>.
          </td>
        </tr>
        <tr>
          <td><span class="code">__pixel x, y, c</span></td>
          <td><span class="code">int, int, colour</span></td>
          <td>N/A</td>
          <td>
            Set the pixel at
            <span class="code">x, y</span>
            to the colour
            <span class="code">c</span>.
          </td>
        </tr>
      </tbody>
    </table>

    <p>
      While the compiler does allow closures (functions which use variables from an outer scope),
      the code generated for them will almost certainly be buggy, as the variable will be stored in
      a frame whose depth relative to the outermost frame of the function will vary depending on
      where the function is called.
    </p>

    <h1>Pixel VM Specification</h1>

    <h2>Pixel VM State</h2>

    <p>The Pixel VM state consists of the following:</p>
    <ul>
      <li>A work stack used in any computation</li>
      <li>A call stack which tracks function calls</li>
      <li>A frame stack which is used to manage scopes</li>
      <li>A graphical display with a fixed width and height</li>
      <li>A log which is used for output</li>
    </ul>

    <p>
      The work stack is used when computing values. It can only be accessed as a stack; in
      particular the program cannot see beyond the topmost element without first popping it off the
      stack.
    </p>

    <p>
      The call stack is used whenever a function call or a return needs to be executed. Each item in
      the stack consists of a local (function-specific) instruction pointer (also called program
      counter or PC), and a reference to the function itself.
    </p>

    <p>
      When a function is called, an item containing a reference to the called function is pushed to
      the stack (with the local PC initialized to 0).
    </p>

    <p>
      When a <span class="code">ret</span> opcode needs to be executed, the topmost item is simply
      popped from the call stack.
    </p>

    <p>
      It should be clear from this description that the topmost item of the call stack acts as a
      sort of global PC, as it contains the current function, as well as a local PC within that
      function. The use of a frame stack instead of a single global PC allows us to avoid needing
      some sort of
      <a href="https://en.wikipedia.org/wiki/Calling_convention" class="link-green"
        >&quot;calling convention&quot;</a
      >.
    </p>

    <p>
      The frame stack consists of frames. Each frame is a resizable array of memory locations. A
      program can use the <span class="code">push</span> opcode to load a value from a memory
      location in any frame in the stack. Without this functionality the Pixel VM would not be
      Turing complete, as its memory would be limited to the work stack. (Perhaps this is not
      entirely true; since one could write a program that uses the Pixel VM display as indexable
      memory.)
    </p>

    <p>
      New frames are created by the <span class="code">call</span> or
      <span class="code">oframe</span> opcodes. New frames will be pushed onto the frame stack.
    </p>

    <p>
      Frames are meant to correspond to scopes in the Pixel language; the Pixel compiler generates
      code that creates new frames when entering control blocks such as an
      <span class="code">if</span> or <span class="code">for</span> statement.
    </p>

    <p>The top frame can be resized using the <span class="code">alloc</span> opcode.</p>

    <p>
      Frames can be popped off the frame stack and destroyed using the
      <span class="code">ret</span> or the <span class="code">cframe</span> opcodes.
    </p>

    <p>
      The display is used to output graphics. The <span class="code">read</span> opcode can be used
      to read a pixel on the display. The <span class="code">pixel</span> and
      <span class="code">pixelr</span> opcodes can be used to colour pixels on the display. It is
      assumed that the display's width and height do not change during execution of a program.
    </p>

    <h2>Data types</h2>

    <p>Each item in the work or frame stack has one of the following types:</p>
    <ul>
      <li>An IEEE754 floating point number.</li>
      <li>A colour (encoded as a hex string, e.g. <span class="code">#7f5612</span>)</li>
      <li>
        An instruction pointer which specifies the offset of some instruction within the current
        function. When a <span class="code">push</span> instruction with a PC relative offset (e.g.
        <span class="code">#PC+9</span>) is executed, the PC relative offset is converted into an
        instruction pointer before pushing onto the work stack. This is necessary since the PC
        relative offset is specified relative to the current PC, which will change by the time we
        actually use the offset.
      </li>
      <li>
        A reference to a function (encoded as a function label, e.g.
        <span class="code">.main</span>)
      </li>
      <li>A pointer to an array of one of the above data types (or another array type)</li>
    </ul>

    <p>
      Memory locations within a frame or an array can also contain a special
      <span class="code">undefined</span> value, meaning they have not yet been initialized.
    </p>

    <p>
      Both the work stack and the frame stack are untyped, meaning any location within them can
      contain a value of any type. Each opcode requires the items it pops from the work stack to
      have a certain type; if one of these expectations is violated, the VM will crash with a type
      error. The expected types for each opcode are tabulated below.
    </p>

    <p>
      Booleans can be encoded as numbers. The logical opcodes treat any non-zero number as true and
      zero as false.
    </p>

    <h2>Initial State</h2>

    <p>
      The VM is initialized with an empty work stack and a frame stack containing a single empty
      frame. The call stack initially holds a single item referencing a special function called
      <span class="code">.main</span> which is the designated entry point for a program.
    </p>

    <h2>Opcodes</h2>

    <p>The following is a comprehensize list of opcodes which are supported by the Pixel VM:</p>

    <table>
      <thead>
        <tr>
          <th>Opcode</th>
          <th>Work Stack Before</th>
          <th>Work Stack After</th>
          <th>Description</th>
        </tr>
      </thead>
      <tbody>
        <tr>
          <td><span class="code">add</span></td>
          <td><span class="code">a b ..</span></td>
          <td><span class="code">a + b ..</span></td>
          <td>
            Pops two numbers off the work stack, and adds them together, pushing the result onto the
            work stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">sub</span></td>
          <td><span class="code">a b ..</span></td>
          <td><span class="code">a - b ..</span></td>
          <td>
            Pops two numbers off the work stack, and subtracts them, pushing the result onto the
            work stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">mul</span></td>
          <td><span class="code">a b ..</span></td>
          <td><span class="code">a * b ..</span></td>
          <td>
            Pops two numbers off the work stack, and multiplies them, pushing the result onto the
            work stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">div</span></td>
          <td><span class="code">a b ..</span></td>
          <td><span class="code">a / b ..</span></td>
          <td>
            Pops two numbers off the work stack, and divides them, pushing the result onto the work
            stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">inc</span></td>
          <td><span class="code">a ..</span></td>
          <td><span class="code">a + 1 ..</span></td>
          <td>
            Pops a number off the work stack, and increments it, pushing the result onto the work
            stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">dec</span></td>
          <td><span class="code">a ..</span></td>
          <td><span class="code">a - 1 ..</span></td>
          <td>
            Pops a number off the work stack, and decrements it, pushing the result onto the work
            stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">or</span></td>
          <td><span class="code">a b ..</span></td>
          <td><span class="code">a &#8744; b ..</span></td>
          <td>
            Pops two numbers off the work stack, computes their logical or (maximum) and pushes the
            result onto the work stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">and</span></td>
          <td><span class="code">a b ..</span></td>
          <td><span class="code">a &#8743; b ..</span></td>
          <td>
            Pops two numbers off the work stack, computes their logical and (minimum) and pushes the
            result onto the work stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">min</span></td>
          <td><span class="code">a b ..</span></td>
          <td><span class="code">min(a,b) ..</span></td>
          <td>
            Pops two numbers off the work stack, computes their minimum and pushes the result onto
            the work stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">max</span></td>
          <td><span class="code">a b ..</span></td>
          <td><span class="code">max(a,b) ..</span></td>
          <td>
            Pops two numbers off the work stack, computes their maximum and pushes the result onto
            the work stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">irnd</span></td>
          <td><span class="code">a ..</span></td>
          <td><span class="code">rand(0,a) ..</span></td>
          <td>
            Pops a number off the work stack, and pushes a random (floating point) number between 0
            and that number (exclusive) onto the work stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">not</span></td>
          <td><span class="code">a ..</span></td>
          <td><span class="code">&#172;a ..</span></td>
          <td>
            Pops a number off the work stack, and pushes its logical negation onto the work stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">lt</span></td>
          <td><span class="code">a b ..</span></td>
          <td><span class="code">a &lt; b ..</span></td>
          <td>
            Pops two numbers <span class="code">a, b</span> off the work stack, compares them, and
            pushes <span class="code">a &lt; b</span> onto the work stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">le</span></td>
          <td><span class="code">a b ..</span></td>
          <td><span class="code">a &lt;= b ..</span></td>
          <td>
            Pops two numbers <span class="code">a, b</span> off the work stack, compares them, and
            pushes <span class="code">a &lt;= b</span> onto the work stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">eq</span></td>
          <td><span class="code">a b ..</span></td>
          <td><span class="code">a == b ..</span></td>
          <td>
            Pops two items <span class="code">a, b</span> (must be of the same type) off the work
            stack, compares them, and pushes <span class="code">a == b</span> onto the work stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">neq</span></td>
          <td><span class="code">a b ..</span></td>
          <td><span class="code">a != b ..</span></td>
          <td>
            Pops two items <span class="code">a, b</span> (must be of the same type) off the work
            stack, compares them, and pushes <span class="code">a != b</span> onto the work stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">gt</span></td>
          <td><span class="code">a b ..</span></td>
          <td><span class="code">a &gt; b ..</span></td>
          <td>
            Pops two numbers <span class="code">a, b</span> off the work stack, compares them, and
            pushes <span class="code">a &gt; b</span> onto the work stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">ge</span></td>
          <td><span class="code">a b ..</span></td>
          <td><span class="code">a &gt;= b ..</span></td>
          <td>
            Pops two numbers <span class="code">a, b</span> off the work stack, compares them, and
            pushes <span class="code">a &gt;= b</span> onto the work stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">dup</span></td>
          <td><span class="code">a ..</span></td>
          <td><span class="code">a a ..</span></td>
          <td>Pops an item off the work stack, and pushes two copies of it onto the work stack.</td>
        </tr>
        <tr>
          <td><span class="code">push x</span></td>
          <td><span class="code">..</span></td>
          <td><span class="code">x ..</span></td>
          <td>
            Only opcode that takes an argument. The argument is pushed onto the stack. Arguments can
            be:
            <ul>
              <li>IEEE754 floating point numbers.</li>
              <li>A colour (encoded as a hex string, e.g. <span class="code">#7f5612</span>).</li>
              <li>
                A reference to a function (encoded as a function label, e.g.
                <span class="code">.main</span>)
              </li>
              <li>
                A PC relative offset (e.g. <span class="code">#PC+9</span>). This is converted into
                an instruction pointer (see above) before pushing onto the work stack.
              </li>
              <li>
                A memory location of the form <span class="code">[offset:frame]</span>. This will
                push onto the work stack the element at the <span class="code">offset</span>th
                position within the <span class="code">frame</span>th frame in the frame stack. Note
                that frame <span class="code">0</span> is the frame at the top of the stack.
              </li>
              <li>
                A memory location of the form <span class="code">[offset]</span>. This is similar to
                the above, except the frame is assumed to be <span class="code">0</span>, i.e. the
                top frame.
              </li>
            </ul>
          </td>
        </tr>
        <tr>
          <td><span class="code">jmp</span></td>
          <td><span class="code">a ..</span></td>
          <td><span class="code">..</span></td>
          <td>
            Pops an instruction pointer off the work stack, and jumps to the location specified by
            the instruction pointer.
          </td>
        </tr>
        <tr>
          <td><span class="code">cjmp</span></td>
          <td><span class="code">a b ..</span></td>
          <td><span class="code">..</span></td>
          <td>
            Pops an instruction pointer and a number off the work stack, and jumps to the location
            specified by the instruction pointer if the number is equal to zero.
          </td>
        </tr>
        <tr>
          <td><span class="code">cjmp2</span></td>
          <td><span class="code">a b ..</span></td>
          <td><span class="code">..</span></td>
          <td>
            Pops an instruction pointer and a number off the work stack, and jumps to the location
            specified by the instruction pointer if the number is not equal to zero.
          </td>
        </tr>
        <tr>
          <td><span class="code">call</span></td>
          <td><span class="code">f n a1 .. an ..</span></td>
          <td><span class="code">..</span></td>
          <td>
            Pops a function label and a number <span class="code">n</span> off the work stack. It
            then pops <span class="code">n</span> items off the work stack, creates a new frame
            containing them, and pushes it onto the work stack. It then pushes a new item containing
            the function label onto the call stack, effectively calling the function with arguments
            <span class="code">a1 .. an</span>.
          </td>
        </tr>
        <tr>
          <td><span class="code">ret</span></td>
          <td><span class="code">..</span></td>
          <td><span class="code">..</span></td>
          <td>Pops one frame off the frame stack, and pops one item off the call stack.</td>
        </tr>
        <tr>
          <td><span class="code">halt</span></td>
          <td><span class="code">..</span></td>
          <td><span class="code">..</span></td>
          <td>Halts the Pixel VM.</td>
        </tr>
        <tr>
          <td><span class="code">alloc</span></td>
          <td><span class="code">a ..</span></td>
          <td><span class="code">..</span></td>
          <td>
            Pops the number <span class="code">a</span> off the work stack and allocates
            <span class="code">a</span> new empty locations at the end of the frame at the top of
            the frame stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">oframe</span></td>
          <td><span class="code">a ..</span></td>
          <td><span class="code">..</span></td>
          <td>
            Pops the number <span class="code">a</span> off the work stack and creates a new frame
            with <span class="code">a</span> empty locations, pushing it onto the frame stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">cframe</span></td>
          <td><span class="code">..</span></td>
          <td><span class="code">..</span></td>
          <td>Pops a frame off the top of the frame stack, destroying it.</td>
        </tr>
        <tr>
          <td><span class="code">st</span></td>
          <td><span class="code">f l v ..</span></td>
          <td><span class="code">..</span></td>
          <td>
            Pops two numbers and an item off the work stack, and stores the item
            <span class="code">v</span> at the <span class="code">l</span>th location of the
            <span class="code">f</span>th frame in the frame stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">delay</span></td>
          <td><span class="code">a ..</span></td>
          <td><span class="code">..</span></td>
          <td>
            Pops a number <span class="code">a</span> off the work stack and sleeps for
            <span class="code">a</span> milliseconds.
          </td>
        </tr>
        <tr>
          <td><span class="code">pixel</span></td>
          <td><span class="code">x y c ..</span></td>
          <td><span class="code">..</span></td>
          <td>
            Pops two numbers <span class="code">x, y</span> and a colour
            <span class="code">c</span> off the work stack and colours the pixel on the screen at
            <span class="code">x, y</span> with colour <span class="code">c</span>.
          </td>
        </tr>
        <tr>
          <td><span class="code">pixelr</span></td>
          <td><span class="code">x y w h c ..</span></td>
          <td><span class="code">..</span></td>
          <td>
            Pops four numbers <span class="code">x, y, w, h</span> and a colour
            <span class="code">c</span> off the work stack and colours a rectangle on the screen of
            width <span class="code">w</span> and height <span class="code">h</span> whose top left
            corner is at <span class="code">x, y</span> with colour <span class="code">c</span>.
          </td>
        </tr>
        <tr>
          <td><span class="code">clear</span></td>
          <td><span class="code">c ..</span></td>
          <td><span class="code">..</span></td>
          <td>Pops a colour off the work stack and colours the entire screen with it.</td>
        </tr>
        <tr>
          <td><span class="code">read</span></td>
          <td><span class="code">x y ..</span></td>
          <td><span class="code">c ..</span></td>
          <td>
            Pops two numbers <span class="code">x, y</span> off the work stack, and pushes the
            colour of the pixel at <span class="code">x, y</span> on the screen onto the work stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">width</span></td>
          <td><span class="code">..</span></td>
          <td><span class="code">w ..</span></td>
          <td>Pushes the width of the screen onto the work stack.</td>
        </tr>
        <tr>
          <td><span class="code">height</span></td>
          <td><span class="code">..</span></td>
          <td><span class="code">h ..</span></td>
          <td>Pushes the height of the screen onto the work stack.</td>
        </tr>
        <tr>
          <td><span class="code">print</span></td>
          <td><span class="code">x ..</span></td>
          <td><span class="code">..</span></td>
          <td>Pops an item off the work stack and prints a representation of it to the log.</td>
        </tr>
        <tr>
          <td><span class="code">alloca</span></td>
          <td><span class="code">a ..</span></td>
          <td><span class="code">b ..</span></td>
          <td>
            Pops a number <span class="code">a</span> off the work stack, creates a new array
            containing <span class="code">a</span> empty locations, and pushes a pointer to it onto
            the work stack.
          </td>
        </tr>
        <tr>
          <td><span class="code">sta</span></td>
          <td><span class="code">a i v ..</span></td>
          <td><span class="code">..</span></td>
          <td>
            Pops a pointer to an array <span class="code">a</span>, a number
            <span class="code">i</span> and an item <span class="code">v</span> off the work stack,
            and sets the <span class="code">i</span>th location in array
            <span class="code">a</span> to value <span class="code">v</span>.
          </td>
        </tr>
        <tr>
          <td><span class="code">lda</span></td>
          <td><span class="code">a i ..</span></td>
          <td><span class="code">a[i] ..</span></td>
          <td>
            Pops a pointer to an array <span class="code">a</span> and a number
            <span class="code">i</span> off the work stack, then pushes the item at the
            <span class="code">i</span>th location in array <span class="code">a</span> onto the
            work stack.
          </td>
        </tr>
      </tbody>
    </table>
  </div>
</template>

<style lang="css" scoped>
.about {
  @apply p-4 max-w-prose md:max-w-screen-md lg:max-w-screen-lg mx-auto;
}

h1 {
  @apply font-extrabold text-xl text-slate-200 my-4;
}

h2 {
  @apply font-extrabold text-lg text-slate-200 my-4;
}

p {
  @apply mb-3;
}

ul {
  @apply list-disc mb-3;
}

li {
  @apply ml-6;
}

table {
  @apply table-auto;
}

th {
  @apply px-2 font-bold;
}

td {
  @apply px-2;
}

thead {
  @apply bg-slate-900;
}

tbody {
  @apply divide-y divide-slate-700;
}

.code {
  @apply font-mono text-slate-200 bg-slate-700 px-1;
}
</style>
