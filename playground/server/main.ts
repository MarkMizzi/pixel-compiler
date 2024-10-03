/* Compiler Server */

import { ExecException, exec } from 'child_process'
import express, { Express, Request, Response } from 'express'
import tmp from 'tmp'
import fs from 'node:fs'
import { StatusCodes } from 'http-status-codes'

interface CompilerInput {
  srcCode: string
}

interface CompilerOutput {
  xmlOutput: string
  asmOutput: string
  compilerStdOut: string
  compilerStdErr: string
}

function validateCompilerInput(x: any): CompilerInput | undefined {
  if (!x || !x.srcCode || typeof x.srcCode != 'string') return undefined
  return x as CompilerInput
}

const app: Express = express()

// port which the web server will listen to
const port = 8080

// setup so that accesses to the root url serve the frontend of the application (which is built under dist/)
app.use(express.static('dist'))

app.get('/compile', function (req: Request, res: Response) {
  const input = validateCompilerInput(req.body)

  if (input === undefined) {
    res.status(StatusCodes.BAD_REQUEST).send('Invalid compilation request.')
    return
  }

  const sourceFile = tmp.fileSync({ mode: 0o644, prefix: 'pixel-src-', postfix: '.pix' })
  const asmOutFile = tmp.fileSync({ mode: 0o644, prefix: 'pixel-asm-', postfix: '.pixardis' })
  const xmlOutFile = tmp.fileSync({ mode: 0o644, prefix: 'pixel-xml-', postfix: '.xml' })

  try {
    fs.writeFileSync(sourceFile.name, input!.srcCode, { encoding: 'ascii' })
  } catch (err) {
    res.status(StatusCodes.INTERNAL_SERVER_ERROR).send('Internal Server Error. Try again later.')
    return
  }

  exec(
    `/pixelc -o ${asmOutFile} -xml ${xmlOutFile} ${sourceFile}`,
    (error: ExecException | null, stdout: string, stderr: string) => {
      if (error !== null) {
        res
          .status(StatusCodes.INTERNAL_SERVER_ERROR)
          .send('Internal Server Error. Try again later.')
        return
      }

      const output: CompilerOutput = {
        asmOutput: fs.readFileSync(asmOutFile.name, { encoding: 'ascii' }),
        xmlOutput: fs.readFileSync(xmlOutFile.name, { encoding: 'ascii' }),
        compilerStdOut: stdout,
        compilerStdErr: stderr
      }

      res.status(StatusCodes.OK).send(output)
    }
  )
})

app.listen(port, function () {
  console.log(`Pixel playground app listening at http://localhost:${port}`)
})
