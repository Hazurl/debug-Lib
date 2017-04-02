# debug-Lib
Librairie de debug en c++ inspiré de Winston (lib nodejs)

## Level de debug

5 niveaux : Error, Warning, Log, Info, Verbose, Special

## Utilisation
Pour écrire des messages dans les niveaux correspondants

```C++
void Logger::error (std::string msg);
void Logger::warning (std::string msg); // ou Logger::warn
void Logger::log (std::string msg);
void Logger::info (std::string msg);
void Logger::verbose (std::string msg); // ou Logger::verb
void Logger::special (std::string msg); // ou Logger::spe
```

## Transport
Permet de rediriger les logs

```C++
void Transport (std::ostream& os);
void Transport (std::string filePath);
```

## Redirection

```C++
void Logger::createTransport(Transport* t, std::string const& name); // ajouter un transport
void Logger::setTransportOf(unsigned int levels, std::string const& name); // fixe les logs spécifié vers un transport
void Logger::addTransportOf(unsigned int levels, std::string const& name); // ajoute le transport aux logs spécifiés
void Logger::removeTransportOf(unsigned int levels, std::string const& name); // retire le transport des logs spécifiés
void Logger::cleanTransportOf(unsigned int levels); // retire tout les transports des logs spécifiés
```

## Section
Afin de définir des sections
```C++
// Commence une section, n'affiche que les logs "levels" et affiche le titre dans "titleLevels"
void Logger::section(std::string name, unsigned int levels = Logger::ALL, unsigned int titleLevels = Logger::ALL);
void Logger::section_end(std::string name); // Termine une section
void Logger::showSection(std::string name); // affiche une section
void Logger::hideSection(std::string name); // masque une section
void Logger::title(std::string name, unsigned int levels); // Affiche un titre sur les logs spécifiés
```

## Affichage

```C++
void Logger::showOnly(unsigned int levels = Logger::ALL); // logs a affiché (global au programme)
void Logger::allowColorOn(std::string const& name, bool allowC); // affiche les couleurs sur un transport
```

### Format

```C++
void Logger::setFormatOf(std::string const& name, std::string const& format); // modifie le format d'un transport
```
```
  {lvl} : le level du log (par exemple "error")
  
  {msg} : le message du log
  
  {col} : debut de la couleur (en fonction du niveau du log (par exemple rouge pour error)
  
  {bld} : debut de la police grasse
  
  {udl} : debut du soulignement
  
  {clr} : termine tout (couleur, police grasse / souligné) 
```
Date / Heure :
```
  {year} : année
  
  {mon} : mois
  
  {day} : jour du mois
  
  {hour} : heures
  
  {min} : minutes
  
  {sec} : secondes
  
  {mil} : milliseconds
  
  {mic} : microseconds
```
Pour fixé une position :

    {x} : avec x = la position par exemple {12}

Format par défaut :

  {col}{bld}{lvl}{clr} {8}- {msg}
  
qui affiche : 
  "error   - mon message"
  
Pour afficher un '%' ou un '\\' il faut les échapper avec un '\\'.

## Exemple

cf : [main.cpp](src/main.cpp)






