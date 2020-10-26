# Mytendance
Mytendance is een Presentie InformatieSysteem (PrIS). Dit systeem automatiseert de huidige werkwijze, waardoor er verschillende processen efficiënter zullen verlopen.

## Inhoudsopgave
- [Benodigheden](#benodigdheden)
- [Git gerelateerd](#git-gerelateerd)
  - [Branches](#branches)
  - [Status](#check-de-status)
  - [Staging area](#staging-area)
  - [Commits](#commits)
  - [Pushen](#pushen)
  - [Git rebase](#git-rebase)
  - [Merge conflicts](#merge-conflicts)
  - [Git Stash](#git-stash)
  - [Commando cheatsheet](#commando-cheatsheet)
  - [Help](#help)
- [GitHub gerelateerd](#github-gerelateerd)
  - [Pull requests](#pull-requests)
  - [User Story en Sub Tasks](#user-story-en-sub-tasks)
  - [Formatting](#formatting)

## Benodigheden
Om het project te gebruiken is het volgende nodig:
- Een Java versie die met het project werkt (onbekend welke Java versie).
- Een lokale installatie van het UI-framework JavaFX.
- Een installatie van Visual Paradigm waarmee de `.vvp` bestanden geopend kunnen worden.

## Git gerelateerd
Voordat je aan een user story/task begint moet je altijd even de laatste veranderingen ophalen (zonder $ natuurlijk):
```
$ git pull
```

### Branches
Nadat je dit gedaan heb, is het handig om een branch aan te maken voor jouw specifieke story/task. Om alles geordend te houden maken we gebruik van een notatie.

Deze notatie is opgebouwd uit drie delen:
- feature/
- ID van de issue
- Korte beschrijving van user story/task

Voorbeeld:
```
$ git branch feature/1-add-login-screen 
```
Let op! Op dit moment heb je alleen maar een branch aangemaakt. dat wil zeggen dat je nog steeds op `master` zit. 

Om naar de nieuwe branch te gaan gebruik je de volgende commando:
```
$ git checkout feature/1-add-login-screen
```
Het kan zijn dat je een branch wil aanmaken en er tegelijkertijd naartoe gaan, dat kan:
```
$ git checkout -b feature/2-add-register-screen
```
de `-b` flag zorgt ervoor dat er gelijk een branche wordt aangemaakt.

### Check de status
Het is handig om eerst te kijken wat voor bestanden er (bijvoorbeeld) aangemaakt zijn, dit kan je doen door het volgende:
```
$ git status
On branch feature/1-add-login-screen
Untracked files:
  (use "git add <file>..." to include in what will be committed)

        README.md

nothing added to commit but untracked files present (use "git add" to track)

```
In dit geval is `README.md` aangemaakt.
In het volgende voorbeeld kan je zien dat er iets is aangepast:
```
$ git status
On branch feature/1-add-login-screen
Changes not staged for commit:
  (use "git add <file>..." to update what will be committed)
  (use "git checkout -- <file>..." to discard changes in working directory)

        modified:   README.md

no changes added to commit (use "git add" and/or "git commit -a")

```

### Staging area
De [staging area](https://softwareengineering.stackexchange.com/a/119790) is een gebied waar de veranderingen (tijdelijk) opstaan. Hier kan je aangeven wat er in de volgende commit mee wordt genomen.

Om deze bestand(en) toe te voegen aan de staging area kan je het volgende gebruiken:
```
$ git add naam_van_bestand
```
Of:
```
$ git add naam_van_directory/
```

Voorbeeld van hoe het er in de staging area er uit ziet:
```
$ git status
On branch feature/1-add-login-screen
Changes to be committed:
  (use "git reset HEAD <file>..." to unstage)

        new file:   README.md

```

### Commits
Nadat je alles toegevoegd hebt aan de staging area, kan je het in een commit stoppen. Commits zijn lokale veranderingen die je in een soort van "moment" opslaat.

Om een commmit te maken kan je het volgende doen:
```
$ git commit
```
Dit opent waarschijnlijk `vim` of `nano` in je shell, dit kan je eventueel aanpassen naar een andere editor. Dit kan er zo uit zien:
```
COMMIT_MESSAGE_HIER
# Please enter the commit message for your changes. Lines starting
# with '#' will be ignored, and an empty message aborts the commit.
#
# On branch feature/1-add-login-screen
# Changes to be committed:
#	new file:   README.md
```
Dit toont een overzicht van alles wat je aan de staging area toegevoegd heb.

Vaak, voor kleine veranderingen, wil je het sneller doen. Dit kan door het volgende te doen:
```
$ git commit -m "Add README.md"
```
Zorg er wel voor dat je duidelijke commit messages gebruikt, deze zijn vaak in het engels. Kijk op <https://wiki.openstack.org/wiki/GitCommitMessages> voor good practices hiervoor.

Nadat je dit gemaakt heb, heb je de lokale verandering opgeslagen. Om alle commits terug te zien kan je (als je Git Bash gebruikt) de volgende commando uitvoeren:
```
$ tig
```
Dit laat een tekstuele weergave zien van al je commits (in de huidige branch!). Je kan hier ook andere tools voor gebruiken zoals:
- Git Kraken
- Sourcetree

Maar dat raad ik af, van de command line kan je meer leren!

### Pushen
Nu we lokale veranderingen opgeslagen hebben in commits, kunnen we het op de remote repository zetten. In dit geval is dat GitHub zelf.

Waarschuwing: voordat je iets pushed, kijk altijd of je niet op de `master` branch zit!

Om te pushen moet je het volgende doen:
```
$ git push
```
Meestal, voor de eerste keer, weet Git nog niet waar die naartoe moet pushen. Als deze commando uitgevoerd wordt, krijg je het volgende te zien:
```
fatal: The current branch test has no upstream branch.
To push the current branch and set the remote as upstream, use

    git push --set-upstream origin test
```
Hier geeft Git al (slim) aan hoe je dit kan oplossen. Nadat je de voorgestelde commando hebt uitgevoerd, zal Git onthouden waarnaartoe die moet pushen

De volgende keren hoef je dus alleen maar `git push` te doen! 

Nadat je gepushed heb kan je een [Pull request](#pull-requests) maken.

### Git rebase
Om te zorgen dat je de laatste veranderingen van `master` in jouw branch krijgt, moet je het volgende doen:

Het is altijd best practice om de laatste changes even van `master` te halen. Eventueel is het ook handig om `git status` te doen, sinds er veranderingen kunnen zien die je wilt opslaan. Dit kan je doen door een tijdelijke commit te maken of door te [stashen.](#git-stash)
```
$ git checkout master
...
$ git pull
```

Hierna moet je weer terug naar de branch waar je oorspronkelijk in zat, doormiddel van een [checkout.](#branches)

Nadat je weer in je eigen branch zit, is het tijd om de changes van master in de desbetreffende branch te zetten. Dit kunnen we doen door het volgende:
```
$ git rebase master
```

Als je al commits had gemaakt zal je zien dat de rebase daarover heen gaat. Als alles goed ging, zal je niet al te veel zien. Meestal, als het fout gaat, krijg je een lap tekst te zien. Als je die lap tekst ziet, kan je er van uit gaan dat er een merge conflict is.

### Merge conflicts
Voor hulp vraag joeri maar, het kan alsnog handig zijn om te weten hoe je de merge conflict kan bekijken.

Om te zien waar Git precies op faalt, kan je Intellij gebruiken. Als je een merge conflict heb moet je linksonderin in Intellij op de `9: Version Control` knop drukken. Hierna wordt er een kleine scherm geopend waarin `Resolve` in het blauw staat. Dit opent een kleine menu met de bestanden die een merge conflict hebben.

 Als je op één van deze bestanden dubbelklikt, opent Intellij de zogenaamde Diff view:

 ![Image](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fwww.tilcode.com%2Fwp-content%2Fuploads%2F2015%2F09%2Fintellij_merge_conflict_tool.png&f=1&nofb=1)

 In het midden kan je het uiteindelijke resultaat zien, aan de linker kant zijn jouw veranderingen en aan de rechterkan zijn(meestal) de veranderingen van `master`

### Git stash
Als je je veranderingen nog niet wil committen, kan het handig zijn om je veranderingen tijdelijk in een "zak" te stoppen. In Git kan dat door het volgende te gebruiken:
```
$ git stash
```
Deze commando slaat het volgende op:
 - Nieuwe bestanden.
 - Added changes.
 - Gemodificeerde bestande.

Het slaat alleen geen `untracked` bestanden op, daar is wel een flag voor:
```
$  git stash -u
```

`-u` zorgt ervoor dat de untracked veranderingen/toevoegingen ook in de "zak" worden opgeslagen.

Om de veranderingen weer terug te krijgen kan je het volgende doen:
```
$ git stash pop 
```

Hiermee kan je de veranderingen weer terugkrijgen.

### Commando cheatsheet
| Commando                                       | Beschrijving                                                                      |
| -----------------------------------------------|-----------------------------------------------------------------------------------| 
| git pull                                       | Haalt de nieuwste verandering van remote (GitHub).                                | 
| git branch                                     | Toont alle branches (lokaal).                                                     |  
| git branch branch_naam                         | Maakt een nieuwe branch met naam.                                                 |  
| git checkout branch_naam                       | gaat in de branch.                                                                | 
| git checkout -b nieuwe_branch_naam             | Korte manier om een nieuwe branch aan te maken en gaat er gelijk in.              | 
| git status                                     | Toont de status: modified, deleted en added changes.                              | 
| git add naam_van_bestand                       | Voegt bestand toe aan staging.                                                    | 
| git add naam_van_directory/                    | Voegt gehele directory aan staging (recursief, alle bestanden in directory).      | 
| git commit                                     | Opent een editor (`nano` of `vim`) om een commit message in te voeren.            | 
| git commit -m "message"                        | Korte manier om een commit aan te maken met message.                              | 
| git push                                       | Pushed gemaakte commits naar remote (GitHub). Check of je niet in master zit!     | 
| git push --set-upstream origin naam_van_branch | Hierdoor weet Git waarnaartoe die moet pushen.                                    | 
| git stash                                      | Sla (bijna) alles tijdelijk op in een "zak".                                      | 
| git rebase branch_naam                         | Baseer de huidige branch op een andere branch.                                    | 
| tig                                            | Toont een tekstuele representatie van de geschiedenis van commits.                | 

### Ik kom er niet uit
Het kan gebeuren dat je niet meer weet wat je moet doen of je hebt iets gesloopt. Als je problemen heb met Git, vraag joeri om hulp!

## GitHub gerelateerd
De functionaliteiten die hier besproken worden zijn specifiek voor GitHub.

### Pull requests
Nadat je hebt gepushed, moet je naar GitHub gaan. In GitHub zie je bovenaan het scherm de branch waarnaartoe je gepushed hebt. GitHub vraagt dan ook of je dan een Pull request wil maken. Dit zou ik ook gaan doen.

Voor elke Pull request **moeten** minstens twee personen gereviewed hebben. We doen dit om het niveau van code kwaliteit zo hoog mogelijk te houden. 

In het Pull request scherm kan je reviewers aan de Pull request toevoegen. De volgende mensen zijn reviewers:
- joeri
- @joeri-hu
- @JortWillemsen

De reviewers kunnen vragen of je bepaalde dingen kan aanpassen, hier kan je veel van leren!

Nadat de twee personen je Pull request goedkeuren, kan er gemerged worden. 
Let op: joeri is de enige die (onofficieel) mag mergen! Hou je hier ook aan a.u.b.

### User Story en Sub Tasks
Een user story zegt wat de gebruiker wil in een korte zin. Omdat dit niet precies zegt wat er in de code moet gebeuren, kunnen we dit preciezer maken door een aantal Sub Tasks te hangen aan de User Story.

Wij hebben als team gekozen om (standaard) de User Stories op te splitsen in de volgende Sub Tasks:
- Klassendiagram
- Schermen
- Klassen implementeren
- Unit test
- Sequence diagram

We gebruiken Issues om meer functionaliteit toe te voegen aan de Sub Task, zoals het assignen van mensen, labels en meer.


### Formatting
Doormiddel van [markdown](https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet) kunnen we gemakkelijk allerlei opmaak toevoegen aan Issues en aan Pull requests.

In de link hierboven staat een soort van cheatsheet die alle mogelijke formatting opties laat zien.