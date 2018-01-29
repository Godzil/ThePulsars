# phpMyAdmin MySQL-Dump
# http://phpwizard.net/phpMyAdmin/
#
# Serveur: localhost Base de données: finale

# --------------------------------------------------------
#
# Structure de la table 'Bitmaps'
#

DROP TABLE IF EXISTS Bitmaps;
CREATE TABLE Bitmaps (
   id int(11) DEFAULT '0' NOT NULL,
   login varchar(8) NOT NULL,
   nom varchar(80) NOT NULL,
   prenom varchar(80) NOT NULL,
   pseudo varchar(80) NOT NULL,
   machine varchar(80) NOT NULL,
   statut int(11) DEFAULT '0' NOT NULL,
   staff tinyint(4) DEFAULT '0' NOT NULL,
   PRIMARY KEY (id),
   UNIQUE pseudo (pseudo),
   UNIQUE login (login)
);


# --------------------------------------------------------
#
# Structure de la table 'Champions'
#

DROP TABLE IF EXISTS Champions;
CREATE TABLE Champions (
   id int(11) NOT NULL auto_increment,
   nom varchar(80) NOT NULL,
   auteur int(11) DEFAULT '0' NOT NULL,
   fichier varchar(255) NOT NULL,
   actif tinyint(4) DEFAULT '1' NOT NULL,
   public tinyint(4) DEFAULT '1' NOT NULL,
   score float DEFAULT '0' NOT NULL,
   best float DEFAULT '0' NOT NULL,
   nbmap int(11) DEFAULT '0' NOT NULL,
   total int(11) DEFAULT '0' NOT NULL,
   tourn tinyint(3) unsigned DEFAULT '0' NOT NULL,
   PRIMARY KEY (id),
   UNIQUE nom (nom),
   KEY bitmap (auteur)
);


# --------------------------------------------------------
#
# Structure de la table 'Maps'
#

DROP TABLE IF EXISTS Maps;
CREATE TABLE Maps (
   id int(11) NOT NULL auto_increment,
   nom varchar(80) NOT NULL,
   descr varchar(255) NOT NULL,
   fichier varchar(255) NOT NULL,
   joueurs tinyint(4) DEFAULT '0' NOT NULL,
   auteur int(11) DEFAULT '0' NOT NULL,
   taille varchar(80) NOT NULL,
   public tinyint(4) DEFAULT '1' NOT NULL,
   actif tinyint(4) DEFAULT '1' NOT NULL,
   PRIMARY KEY (id),
   UNIQUE nom (nom),
   KEY fichier (fichier)
);


# --------------------------------------------------------
#
# Structure de la table 'Matchs'
#

DROP TABLE IF EXISTS Matchs;
CREATE TABLE Matchs (
   id int(11) NOT NULL auto_increment,
   map int(11) DEFAULT '0' NOT NULL,
   host varchar(80) NOT NULL,
   port smallint(6) DEFAULT '0' NOT NULL,
   statut smallint(4) DEFAULT '1' NOT NULL,
   joueurs smallint(4) DEFAULT '0' NOT NULL,
   auteur int(11) DEFAULT '0' NOT NULL,
   public tinyint(4) DEFAULT '1' NOT NULL,
   PRIMARY KEY (id),
   KEY map (map)
);


# --------------------------------------------------------
#
# Structure de la table 'Msg'
#

DROP TABLE IF EXISTS Msg;
CREATE TABLE Msg (
   id tinyint(4) DEFAULT '0' NOT NULL,
   msg char(3) NOT NULL,
   PRIMARY KEY (id)
);

#
# Contenu de la table 'Msg'
#

INSERT INTO Msg VALUES ( '0', 'Non');
INSERT INTO Msg VALUES ( '1', 'Oui');

# --------------------------------------------------------
#
# Structure de la table 'Scores'
#

DROP TABLE IF EXISTS Scores;
CREATE TABLE Scores (
   id int(11) NOT NULL auto_increment,
   match_id int(11) DEFAULT '0' NOT NULL,
   champion int(11) DEFAULT '0' NOT NULL,
   score float DEFAULT '-1' NOT NULL,
   PRIMARY KEY (id)
);