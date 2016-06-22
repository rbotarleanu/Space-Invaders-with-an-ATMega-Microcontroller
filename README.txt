Botarleanu Robert-Mihai 331CB
Space Invaders - PM 2016

	Acest proiect consta in implementarea jocului Space Invaders cu ajutorul
unui microcontroller ATMega, afisajul facandu-se pe LCD, iar controlul prin
intermediul unui buton de tragere si a unui slider qtouch.

	Fisierele ce se pot regasi au urmatoarea semnificatie:
		- src/*: codul sursa pentru qtouch, nemodificat
		- touch.h/touch.c: din code sample-ul PM 2015 pentru qtouch
		- lcd.h/lcd.c: libraria pentru LCD-ul grafic de 128x128px
		- utils.h: macrodefinitii utile
		- space_invaders.h/.c: functii, structuri de date si macrodefinitii
			utile pentru randarea jocului space invaders pe ecran
		- main.c: logica principala a jocului
		- list.h/list.c: implementarea unei liste simplu inlantuite generice
		- gfx.h/.c: librarie de grafica (pentru lcd, inclusa cu lcd.h/.c)
		- glcdfont.c: pentru randare de font pe ecran
		
	Jocul se deruleaza pe mai multe runde, jucatorul putand avea un maxim de 1 glont
activ pe teren. Extraterestii vor trage inapoi (aleator) la fiecare 2/1 trageri ale
jucatorului, cate 1,2,4,.. extraterestrii la orice moment.

	Asteroizii sunt destructibili, atat de catre jucator, cat si de catre navele
inamice.

	Nu exista un numar maxim de runde, jocul continua pana cand jucatorul pierde.
Pe partea de sus a ecranului se pot regasi scorul jujcatorului curent si scorul
cel mai mare din instanta curenta de joc.

	Randarea pe ecran este realizata intr-o maniera optimizata: se redeseneaza
doar zonele ce au suferit modificari de pozitie - cu alte cuvinte, numai gloantele
si jucatorul vor fi redesenate, fapt ce conduce la o perfomanta imbunatatita si
la eliminarea intarzierilor cauzate de randarea inceata a LCD-ului.

	Exista un numar de 4 asteroizi si 10 nave extraterestere (pe 2 randuri a cate 5),
navele se vor reseta la fiecare runda - dar asteroizii o data ce au fost distrusi vor
fi repopulati doar la inceperea unei noi sesiuni (cand se reseteaza scorul la 0).
	
	Din cauza faptului ca libraria de qtouch folosita necesita dezactivarea pull-upurilor
pe durata masurarii, ca work-around am ales sa dezactivez (folosind bitul PUD din MCU 
Control Register), sa realizez masuratoarea efectiva si sa reactivez rezistentele de pull-up
(specific pe cea a butonului) dupa masurare.
	
	Butonul folosit este cel corespunzator pinului PD6, iar led-ul PD7 ofera feedback
vizual (impreuna cu cel auditiv al buzzer-ului) in momentul in care un jucator trage.
	
	Mai multe detalii legate de implementarea fiecarei functionalitati se pot
regasii in comentariile codului.