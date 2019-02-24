'use strict';

var express = require('express'),
    exphbs  = require('express-handlebars'); // "express-handlebars"

var app = express();

app.engine('handlebars', exphbs({defaultLayout: 'main'}));
app.set('view engine', 'handlebars');


app.get('/registrations', function (req, res) {
    res.send("Get registrations.");
});

app.post('/registrations', function(req, res) {
    res.send("Post registrations.");
  });

app.listen(3000, function () {
    console.log('listening on: 3000');
});