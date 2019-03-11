
var mysql = require('mysql');

exports.pool = mysql.createPool({
    host: 'localhost',
    user: 'root',
    password: '797Spin797!',
    database: 'cps401_library'
});
