'use strict'

var db = require('../config/db');

function User(username, pw) {
    this.Username = username
    this.Password = pw
}

User.Authenticate = function (username,pw,callback) {
    db.pool.getConnection(function (err, connection) {
        connection.query(`select * from users where Username = ? and Password = ?`,[username,pw],function (err, data) {
            connection.release()              
            if (err) return callback(err)

            if (data.length > 0) {

                let item = data[0]
                let results = new User(item.Username, item.Password)
                callback(null, results)
                
            } else {
                callback("No records found.", null)
            }
        })
    })
}

module.exports = User