import telebot
import time
from datetime import date

bot = telebot.TeleBot("114919724:AAG9NIEkWZK0Mlboa8aK5sCZLb9Hs_WTjVs")


@bot.message_handler(commands=['start', 'help'])
def send_welcome(message):
	bot.reply_to(message, "Howdy, how are you doing?")
	print message
@bot.message_handler(commands=['home'])
def send_back(message):
	day=date.today().weekday()
	if day==6:
		bot.reply_to(message,"Vado a giocare a basket, per cena ci sono")
@bot.message_handler(func=lambda message: True)
def echo_all(message):
    bot.reply_to(message, message.text)

bot.polling()
