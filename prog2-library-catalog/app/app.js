let express = require('express')
let path = require('path')
let favicon = require('serve-favicon')
let logger = require('morgan')
let cookieParser = require('cookie-parser')
let bodyParser = require('body-parser')
let expressHbs = require('express-handlebars')
let session = require('express-session')
let flash = require('connect-flash')
let validator = require('express-validator')
let routes = require('./routes/index')
let parseurl = require('parseurl')
let crypto = require('crypto')

let app = express()

// view engine setup

app.engine('.hbs', expressHbs({extname: '.hbs'}))
app.engine('.hbs', expressHbs({defaultLayout: 'main'}))
app.set('view engine', '.hbs')

app.use(express.static('views/resc'))

// uncomment after placing your favicon in /public
//app.use(favicon(path.join(__dirname, 'public', 'favicon.ico')));
app.use(logger('dev'))
app.use(bodyParser.json())
app.use(bodyParser.urlencoded({ extended: false }));
app.use(validator())
app.use(cookieParser())
app.use(session({secret: 'mysupersecret', resave: false, saveUninitialized: false}))

// Authentication middleware
app.use(function(req,res,next) {
  if (!req.session.paths) req.session.paths = []
  req.session.paths.push(req.url)

  if (req.session.user) return next()
  
  if (!req.url.includes("maintain")) return next()

  res.redirect("login?error=You must login to edit entry")
})

// Configure HASH
routes.secret = "library_catalog"

app.use(flash())
app.use(express.static(path.join(__dirname, 'public')))

app.use('/', routes);

// catch 404 and forward to error handler
app.use(function(req, res, next) {
  var err = new Error('Not Found')
  err.status = 404
  next(err)
})

// error handlers

// development error handler
// will print stacktrace
if (app.get('env') === 'development') {
  app.use(function(err, req, res, next) {
    res.status(err.status || 500)
    res.render('error', {
      message: err.message,
      error: err
    })
  })
}

// production error handler
// no stacktraces leaked to user
app.use(function(err, req, res, next) {
  res.status(err.status || 500)
  res.render('error', {
    message: err.message,
    error: {}
  })
})

module.exports = app