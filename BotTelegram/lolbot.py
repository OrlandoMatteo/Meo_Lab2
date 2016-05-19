import telebot
import urllib2

bot = telebot.TeleBot("164452241:AAEUeRhzFUHC3baeP7wgoAJkSpqmyNsMdKA")

@bot.message_handler(func=lambda message: True)
def echo_all(message):
	champion=str(message.text)
	itemlist=[]
	link='http://www.probuilds.net/champions/details/'+champion
	response = urllib2.urlopen(link)
	html = response.read()
	test=html[html.find("<h2>Popular Items</h2>"):html.find("<h2>Popular Summoners Spells</h2>")]
	for i in range(1,7):
		items_1=test.split("<div class='bigData'>")
		oggetti=items_1[i].split('"')
		itemlist.append(str(oggetti[3]))
	testo=', '.join(itemlist)
	bot.reply_to(message, testo)

bot.polling()