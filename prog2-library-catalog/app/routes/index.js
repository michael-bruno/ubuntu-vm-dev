var express = require('express');
var router = express.Router();

var Item = require('../models/item');

/* GET home page. */
router.get('/', function (req, res, next) {
    res.render('search');    
});

router.get('/search', function (req, res, next) {
    let title = req.query.Title
    Item.Search(title,function(err,results) {
        res.render("search",{results: results})
    })
});

router.get('/error', function (req, res, next) {
    res.render('error');     
});

module.exports = router;
