'use strict'

var db = require('../config/db')

function Item(id, callNo, author, title, pubInfo, descript, series, addAuthor, updateCount) {
    this.id = id
    this.callNo = callNo
    this.author = author
    this.title = title
    this.pubInfo = pubInfo
    this.descript = descript
    this.series = series
    this.addAuthor = addAuthor
    this.updateCount = updateCount
}

Item.SearchAll = function (callback) {
    db.pool.getConnection(function (err, connection) {
        connection.query('select * from items order by title limit 10', function (err, data) {

            connection.release()              
            if (err) return callback(err)

            if (data) {
                var results = []

                for (var i = 0; i < data.length; ++i) {
                    var item = data[i]
                    results.push(new Item(item.ID, item.CALLNO, item.AUTHOR, item.TITLE, item.PUB_INFO,
                        item.DESCRIPT, item.SERIES, item.ADD_AUTHOR, item.UPDATE_COUNT))
                }
                callback(null, results)
            } else {
                callback(null, null)
            }
        })
    })
}

Item.Search = function (title,start,callback) {
    db.pool.getConnection(function (err, connection) {
        if (err) { connection.release(); return callback(err) }
        connection.query(`select id from items where title like ? order by title`,"%"+title+"%", function (err, data) {
    
            if (err) return callback(err)

            if (data) {
                let ids = []
                data.forEach(i => {
                    ids.push(i.id)
                })

                connection.query(`select * from items where id in (?) order by title limit ?,10`,[ids,start], function (err, data) {
                    let results = []
                    if (data) {

                        for (var i = 0; i < data.length; ++i) {
                            var item = data[i]
                            results.push(new Item(item.ID, item.CALLNO, item.AUTHOR, item.TITLE, item.PUB_INFO,
                                item.DESCRIPT, item.SERIES, item.ADD_AUTHOR, item.UPDATE_COUNT))
                            }
                    }
                    connection.release()
                    return callback(null,results,ids.length)
                })
            }
            else return callback(null,null)
        })
    })
}

Item.SearchByID = function (id,callback) {
    db.pool.getConnection(function (err, connection) {
        connection.query('select * from items where id = (?)',id,function (err, data) {

            connection.release()           
            if (err) return callback(err)

            if (data) {
                let item = data[0]
                let results = new Item(item.ID, item.CALLNO, item.AUTHOR, item.TITLE, item.PUB_INFO,
                    item.DESCRIPT, item.SERIES, item.ADD_AUTHOR, item.UPDATE_COUNT)
                callback(null, results)
                }
            else {
                callback(null, null)
            }
        })
    })
}

Item.Update = function(item,callback) {
    db.pool.getConnection(function (err, connection) {
        let query = `
        update items
        set CALLNO = ?, AUTHOR = ?,
            TITLE = ?, PUB_INFO = ?,
            DESCRIPT = ?, SERIES = ?,
            ADD_AUTHOR = ?, UPDATE_COUNT = ?
        where ID = ? and UPDATE_COUNT = ?
        `
        connection.query(query,[item.callNo,item.author,item.title,item.pubInfo,
            item.descript,item.series,item.addAuthor,parseInt(item.updateCount)+1,item.id,item.updateCount],
            function (err, data) {
            connection.release()

            if (err) return callback(err)

            if (data) {
                return callback(null, data)
                }
            else {
                callback(null, null)
            }
        })
    })
}

module.exports = Item