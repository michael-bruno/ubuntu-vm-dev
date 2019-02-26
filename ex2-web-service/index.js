'use strict';

let PORT = 3000

function print(str) { console.log(str) }

let bodyParser = require('body-parser')
let mysql = require("mysql")
let express = require('express'),
    exphbs = require('express-handlebars')

let pool = mysql.createPool({    
    "host": "localhost",
    "user": "root",
    "password": "797Spin797",
    "database": "registrations",
    "connectionLimit": 10
})

let app = express()
app.use(bodyParser.urlencoded({ extended: false }))
app.use(bodyParser.json())

app.engine('handlebars', exphbs({defaultLayout: 'main'}))
app.set('view engine', 'handlebars');

app.get('/registrations', function (req, res) {
    pool.getConnection(function(err, connection) {
        if (err) { return res.send("An error has occured: " + err)  }

        connection.query("select * from registrations.Contestant", function (err, results) {
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
    
    if (![firstName,lastName,grade,email,shirtSize,hrUsername].every(Boolean)) { return res.status(400).send("Missing required data field(s).") }

    if (!["s","m","l"].includes(shirtSize.toLowerCase())) { return res.status(400).send(`'${shirtSize}' is not a valid shirt size.`) }
    if (!["9","10","11","12"].includes(grade)) { return res.status(400).send(`'${grade}' is not a valid grade.`) }

    pool.getConnection(function(err, connection) {
        if (err) { return res.status(500).send("An error has occured: " + err) }

        let values = [firstName,lastName,grade,email,shirtSize,hrUsername]
        
        let query = "insert into registrations.Contestant(firstName, lastName, grade, email, shirtSize, hrUsername) values (?,?,?,?,?,?)"
        connection.query(query,values,function (err, results) {
            if (err) { return res.status(500).send("An error has occured: " + err) }
            
            res.status(200).send("Record successfully inserted.")
            print("Record successfully inserted.")
            connection.release()
        });
    });
})

app.listen(PORT, function() {
    print(`Listening on ${PORT}.`)
})