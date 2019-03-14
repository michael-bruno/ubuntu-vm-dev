let express = require('express')
let router = express.Router()

let Item = require('../models/item')
let User = require('../models/user')

let crypto = require('crypto')

let REGEX = /[\ 0-9A-Za-z'?]+/g

let hashf = function(pw) {
    return crypto.createHmac('sha256',router.secret).update(pw).digest('hex')
}

let signed_in = function(req) {
    let login = "Login"
    if (verify(req)) login = "Logout"
    return login
}

let verify = function(req) {
    if (!req.session.user) return false
    return req.session.user.Username
}

let logout = function(req) { req.session.user = null }

let render_search = function (req,res,page,err_page) {
    let title = req.query.title

    let start = parseInt(req.query.start) || 0
    let first = start == 0

    let prev_page = start - 10
    let next_page = start + 10

    let matches = title.match(REGEX)

    if (!matches) return res.render(err_page,{LOGIN: signed_in(req), error: "Nothing entered",max: 0,on_page: 0,pages: 0})
    if (matches.join("") != title) return res.render(err_page,{LOGIN: signed_in(req),error: "Invalid characters",max: 0,on_page: 0,pages: 0})

    Item.Search(title,start,function(err,results,max) {
        let on_page = (start/10) + 1
        let pages = Math.ceil(max/10)

        let no_next = on_page == pages || on_page > pages

        res.render(page,{
            LOGIN: signed_in(req),
            item_search: title,
            results: results,
            start: start,
            first: first,
            prev_page: prev_page,
            next_page: next_page,
            max: max,
            on_page: on_page,
            pages: pages,
            no_next: no_next,
        })
    })
}

router.get('/mobile_search', function (req, res, next) {
    res.render('mobile_search',{
        LOGIN: signed_in(req),
        max: 0,
        on_page: 0,
        pages: 0
    })
})

router.get('/mobile_searchresult', function (req, res, next) {
    render_search(req,res,"mobile_searchresult",'mobile_search')
})

/* GET home page. */
router.get('/', function (req, res, next) {
    res.render('search',{
        LOGIN: signed_in(req),
        max: 0,
        on_page: 0,
        pages: 0
    })
})

router.get('/search', function (req, res, next) {
    render_search(req,res,"search","search")
})

router.get('/details', function (req, res, next) {
    let id = req.query.book_id || req.body.book_id
    let search_title = req.query.title
    let success = req.query.success

    Item.SearchByID(id, function(err,r) {
        res.render('details',{
            LOGIN: signed_in(req),
            search_title: search_title,
            id: id,
            title: r.title,
            author: r.author,
            addAuthor: r.addAuthor,
            callNo: r.callNo,
            pubInfo: r.pubInfo,
            descript: r.descript,
            series: r.series,
            update_count: r.updateCount,
            success: success
        })
    })
})

router.post("/details", function (req, res, next) {
    let id = req.query.book_id || req.body.book_id

    Item.SearchByID(id, function(err,r) {
        res.render('details',{
            LOGIN: signed_in(req),
            id: id,
            title: r.title,
            author: r.author,
            addAuthor: r.addAuthor,
            callNo: r.callNo,
            pubInfo: r.pubInfo,
            descript: r.descript,
            series: r.series,
            update_count: r.updateCount
        })
    })
})

router.get('/maintain', function (req, res, next) {

    let id = req.query.book_id
    let success = req.query.success
    let error = req.query.error

    Item.SearchByID(id, function(err,r) {
        res.render('maintain',{
            LOGIN: signed_in(req),
            id: id,
            title: r.title,
            author: r.author,
            addAuthor: r.addAuthor,
            callNo: r.callNo,
            pubInfo: r.pubInfo,
            descript: r.descript,
            series: r.series,
            update_count: r.updateCount,
            error: error,
            success: success
        })
    })
})

router.post('/maintain', function (req, res, next) {
    let title = req.body.txtTitle
    let author = req.body.txtAuthor
    let add_author = req.body.txtAddAuthor
    let call_no = req.body.txtCallNo
    let pub_info = req.body.txtPubInfo
    let descr = req.body.txtDesc
    let series = req.body.txtSeries
    let uc = req.body.uc
    let id =  req.body.book_id

    let back = req.header("Referer")
    let query = ""

    if (!(call_no && author && title)) {
        if (!back.includes("error")) query = "&error=Missing required information"
        return res.redirect(back+query)
    }

    let item = new Item(id,call_no,author,title,pub_info,descr,series,add_author,uc)

    Item.Update(item,function(err,results) {

        if (!back.includes("success") || req.query.success) query = "&success=Record successfully changed"
        if (results.affectedRows == 0) {
            query = "&error=Record changed by another user"
            return res.redirect(`maintain?book_id=${id}`+ query)
        }

        res.redirect(`/details?book_id=${id}`+ query)
    })
})

router.get('/login',function (req, res, next) {
    if (signed_in(req) == "Logout") logout(req)
    
    res.render("login",{LOGIN: signed_in(req), error: req.query.error})
})

router.post('/login',function (req, res, next) {
    let username = req.body.username
    let pw = req.body.password

    if (!(username && pw)) return res.render("login",{LOGIN: signed_in(req),error: "Missing information"})

    let hashed = hashf(pw)
    User.Authenticate(username,hashed,function(err,results) {
        if (err) return res.render("login",{LOGIN: signed_in(req),error: err})

        let username_r = results.Username
        let pw_r = results.Password

        if (!(hashed === pw_r)) return res.render("login",{LOGIN: signed_in(req),error: "Incorrect Username/Password"})

        req.session.user = {}
        req.session.user.Username = username_r

        // Checks to see if user was trying to edit an entry
        let paths = req.session.paths
        if (paths.length >= 3) {
            if (paths[paths.length-1].includes("login") && paths[paths.length-2].includes("error") && paths[paths.length-3].includes("maintain"))
            {
                return res.redirect(paths[paths.length-3])
            }
        }

        res.redirect("/")
    })
})

module.exports = router