# Exercise 2: Web Service

## Curl Commands

Display existing data
* curl -X GET http://172.31.95.166:3000/registrations

Insert new entry
* curl -d "firstName=Joe&lastName=Shmoe&grade=11&email=curl@email.com&shirtSize=m&hrUsername=curlusr" -X POST http://172.31.95.166:3000/registrations

## Notes
Program startup implemented using Cron. (crontab -e)
* (Inside Cron file) "@reboot (sleep 5; sh _/location/script.sh_)"