/* Start the server app */

import express, { Express } from 'express'

const app: Express = express()

// port which the web server will listen to
const port = 8080

// setup so that accesses to the root url serve the frontend of the application (which is built under dist/)
app.use(express.static('dist'))

app.listen(port, function () {
  console.log(`Pixel playground app listening at http://localhost:${port}`)
})
