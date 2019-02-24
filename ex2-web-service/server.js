'use strict';

var express = require('express'),
    exphbs  = require('express-handlebars'); // "express-handlebars"

var app = express();

app.engine('handlebars', exphbs({defaultLayout: 'main'}));
app.set('view engine', 'handlebars');


app.get('/', function (req, res) {
    res.render('home', { inventory: inventory });
});

app.post('/hello', function(req, res) {
    var name = req.body.fname || "world";
    
    res.send("Hello, " + name + "!");
  });

app.listen(3000, function () {
    console.log('express-handlebars example server listening on: 3000');
});