import lolitem
import urllib2

class ItemList(object):
	"""docstring for ItemList"""
	def __init__(self,champion):
		link=urllib2.urlopen('http://www.probuilds.net/champions/details/'+champion)
		html=response.read()
		for c in range(5)
			s1=html[html.find("<h2>Popular Items</h2>"):html.find("<h2>Popular Summoners Spells</h2>")]
			s2=s1.split("<div class='bigData'>")
			s3=s2.split('"')
		
