import urllib2

itemlist=[]
response = urllib2.urlopen('http://www.probuilds.net/champions/details/Akali')
html = response.read()
test=html[html.find("<h2>Popular Items</h2>"):html.find("<h2>Popular Summoners Spells</h2>")]
for i in range(1,7):
	items_1=test.split("<div class='bigData'>")
	oggetti=items_1[i].split('"')
	itemlist.append(oggetti[3])
print itemlist