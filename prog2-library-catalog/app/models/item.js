'use strict';

var db = require('../config/db');

function Item(id, callNo, author, title, pubInfo, descript, series, addAuthor, updateCount) {
    this.id = id;
    this.callNo = callNo;
    this.author = author;
    this.title = title;
    this.pubInfo = pubInfo;
    this.descript = descript;
    this.series = series;
    this.addAuthor = addAuthor;
    this.updateCount = updateCount;
}

Item.SearchAll = function (callback) {
    db.pool.getConnection(function (err, connection) {
        connection.query('select * from items order by title limit 10', function (err, data) {
            //console.log(data);
            connection.release();              
            if (err) return callback(err);

            if (data) {
                var results = [];

                for (var i = 0; i < data.length; ++i) {
                    var item = data[i];
                    results.push(new Item(item.ID, item.CALLNO, item.AUTHOR, item.TITLE, item.PUB_INFO,
                        item.DESCRIPT, item.SERIES, item.ADD_AUTHOR, item.UPDATE_COUNT));
                }
                callback(null, results);
            } else {
                callback(null, null);
            }
        });
    });
}

Item.Search = function (title,start,callback) {
    db.pool.getConnection(function (err, connection) {
        connection.query('select id from items where title like ? order by title limit ? 10',title + "%",start, function (err, data) {
    
            if (err) return callback(err)

            if (data) {
                let ids = []
                data.forEach(i => {
                    ids.push(i.id)
                });

                connection.query(`select * from items where id in (${ids}) order by title`,ids, function (err, data) {
                    let results = []
                    for (var i = 0; i < data.length; ++i) {
                        var item = data[i]
                        results.push(new Item(item.ID, item.CALLNO, item.AUTHOR, item.TITLE, item.PUB_INFO,
                            item.DESCRIPT, item.SERIES, item.ADD_AUTHOR, item.UPDATE_COUNT))
                    }
                    connection.release()
                    return callback(null,results)
                })
            }
            else return callback(null,null)
        })
    });
}

module.exports = Item;