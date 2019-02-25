'use strict';

function print(str) { console.log(str) }

let bodyParser = require('body-parser')
let mysql = require("mysql")
let express = require('express'),
    exphbs  = require('express-handlebars') // "express-handlebars"

let pool = mysql.createPool({    
    "host": "localhost",
    "user": "root",
    "password": "797Spin797!",
    "database": "registrations",
    "connectionLimit": 10
})

let app = express()
app.use(bodyParser.urlencoded({ extended: false }))
app.use(bodyParser.json())

app.engine('handlebars', exphbs({defaultLayout: 'main'}));
app.set('view engine', 'handlebars');

app.get('/registrations', function (req, res) {
    pool.getConnection(function(err, connection) {
        if (err) { return res.send("An error has occured: " + err)  }

        connection.query("select * from registrations.contestant", function (err, results) {
            res.send(results)
            connection.release()
        })
    })
})

app.post('/registrations', function(req, res) {
    let firstName = req.body.firstName
    let lastName = req.body.lastName
    let grade = req.body.grade
    let email = req.body.email
    let shirtSize = req.body.shirtSize
    let hrUsername = req.body.hrUsername
    
    let valid = ([firstName,lastName,grade,email,shirtSize,hrUsername].every(Boolean))

    if (!valid) { return res.status(400).send("Missing required data field(s).") }

    pool.getConnection(function(err, connection) {
        if (err) { return res.status(503).send("An error has occured: " + err) }

        let query = "insert into registrations.contestant(firstName, lastName, grade, email, shirtSize, hrUsername) values (?,?,?,?,?,?)"
        let values = [firstName,lastName,grade,email,shirtSize,hrUsername]

        connection.query(query,values,function (err, results) {
            if (err) { return res.status(500).send("An error has occured: " + err) }
            
            res.status(200).send("OK")
            print("1 record inserted.")
            connection.release()
        });
    });
})

app.listen(3000, function() {
    print('Listening on 3000.')
})