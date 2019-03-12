let express = require('express')
let router = express.Router()

let Item = require('../models/item')

let REGEX = /[\ 0-9A-Za-z'?]+/g

/* GET home page. */
router.get('/', function (req, res, next) {
    res.render('search',{
        max: 0,
        on_page: 0,
        pages: 0
    });    
});

router.get('/search', function (req, res, next) {
    let title = req.query.title
    let start = parseInt(req.query.start) || 0
    let first = start == 0

    let prev_page = start - 10
    let next_page = start + 10

    let matches = title.match(REGEX)

    if (!matches) return res.render("search",{error: "Nothing entered",max: 0,on_page: 0,pages: 0})
    if (matches.join("") != title) return res.render("search",{error: "Invalid characters",max: 0,on_page: 0,pages: 0})

    Item.Search(title,start,function(err,results,max) {
        let on_page = (start/10) + 1
        let pages = Math.ceil(max/10)

        let no_next = on_page == pages || on_page > pages

        res.render("search",{
            title: title,
            results: results,
            start: start,
            first: first,
            prev_page: prev_page,
            next_page: next_page,
            max: max,
            on_page: on_page,
            pages: pages,
            no_next: no_next
        })
    })
});

router.get('/details', function (req, res, next) {
    let id = req.query.book_id

    Item.SearchByID(id, function(err,r) {

        res.render('details',{
            id: id,
            title: r.title,
            author: r.author,
            addAuthor: r.addAuthor,
            callNo: r.callNo,
            pubInfo: r.pubInfo,
            descript: r.descript,
            series: r.series
        });  
    })
});


router.get('/maintain', function (req, res, next) {
    res.render("maintain")
});

module.exports = router;
