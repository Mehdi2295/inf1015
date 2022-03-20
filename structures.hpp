// Solutionnaire du TD3 INF1015 hiver 2021
// Par Francois-R.Boyer@PolyMtl.ca
#pragma once
// Structures mémoires pour une collection de films.

#include <string>
#include <memory>
#include <functional>
#include <cassert>
#include "gsl/span"

using gsl::span;
using namespace std;

struct Film; struct Acteur; // Permet d'utiliser les types alors qu'ils seront défini après.

class ListeFilms {
public:
	ListeFilms() = default;
	void ajouterFilm(Film* film);
	void enleverFilm(const Film* film);
	shared_ptr<Acteur> trouverActeur(const string& nomActeur) const;
	span<Film*> enSpan() const;
	int size() const { return nElements; }
	void detruire(bool possedeLesFilms = false);
	Film*& operator[] (int index) { return elements[index]; }
	Film* trouver(const function<bool(const Film&)>& critere) {
		for (auto& film : enSpan())
			if (critere(*film))
				return film;
		return nullptr;
	}

private:
	void changeDimension(int nouvelleCapacite);

	int capacite = 0, nElements = 0;
	Film** elements = nullptr; // Pointeur vers un tableau de Film*, chaque Film* pointant vers un Film.
};

template <typename T>
class Liste {
public:
	Liste() = default;
	explicit Liste(int capaciteInitiale) :  // explicit n'est pas matière à ce TD, mais c'est un cas où c'est bon de l'utiliser, pour ne pas qu'il construise implicitement une Liste à partir d'un entier, par exemple "maListe = 4;".
		capacite_(capaciteInitiale),
		elements_(make_unique<shared_ptr<T>[]>(capacite_))
	{
	}
	Liste(const Liste<T>& autre) :
		capacite_(autre.nElements_),
		nElements_(autre.nElements_),
		elements_(make_unique<shared_ptr<T>[]>(nElements_))
	{
		for (int i = 0; i < nElements_; ++i)
			elements_[i] = autre.elements_[i];
	}
	Liste(Liste<T>&&) = default;  // Pas nécessaire, mais puisque c'est si simple avec unique_ptr...
	Liste<T>& operator= (Liste<T>&&) noexcept = default;  // Utilisé pour l'initialisation dans lireFilm.

	void ajouter(shared_ptr<T> element)
	{
		assert(nElements_ < capacite_);  // Comme dans le TD précédent, on ne demande pas la réallocation pour ListeActeurs...
		elements_[nElements_++] = move(element);
	}

	// Noter que ces accesseurs const permettent de modifier les éléments; on pourrait vouloir des versions const qui retournent des const shared_ptr, et des versions non const qui retournent des shared_ptr.
	shared_ptr<T>& operator[] (int index) const { return elements_[index]; }
	span<shared_ptr<T>> enSpan() const { return span(elements_.get(), nElements_); }

private:
	int capacite_ = 0, nElements_ = 0;
	unique_ptr<shared_ptr<T>[]> elements_; // Pointeur vers un tableau de Acteur*, chaque Acteur* pointant vers un Acteur.
};
using ListeActeurs = Liste<Acteur>;

class Affichable {

public:

	virtual string afficher() = 0;
};

class Item : public Affichable {
public:
	Item() = default;

	Item(const string titre, const int annee) {
		titre_ = titre;
		annee_ = annee;
	}

	int annee_ = 0;
	string titre_;

	virtual string afficher() {
		return titre_ + " est sorti en " + std::to_string(annee_);
	}

	Item* operator<< (ostream&& os) {
		os << afficher();
		return this;
	}
};

class Film : virtual public Item
{
public:
	Film() = default;
	Film(string titre, string realisateur) { titre_ = titre; realisateur_ = realisateur; }
	Film(const Film& f) : Item(f.titre_, f.annee_), realisateur_(f.realisateur_) {}
	string realisateur_; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
	int anneeSortie = 0, recette = 0; // Année de sortie et recette globale du film en millions de dollars
	ListeActeurs acteurs_;

	virtual string afficher() {
		return titre_ + " est sorti en " + std::to_string(annee_) + ", ce film a ete realisé par " + realisateur_ + " et a genéré " + std::to_string(recette) + "$";
	}

	virtual Item* operator<< (ostream& os) {
		os << afficher();
		return this;
	}
};

struct Acteur
{
	string nom; int anneeNaissance = 0; char sexe = '\0';
};


class Livre : virtual public Item
{
public:
	Livre() = default;
	Livre(const Livre& l) : Item(l.titre_, l.annee_), recette_(l.recette_), nombrePages_(l.nombrePages_) {}
	string auteur_;
	int recette_ = 0;
	int nombrePages_ = 0;

	virtual string afficher() {
		return titre_ + " est sorti en " + std::to_string(annee_) + ", ce livre a été écrit par " + auteur_ + " ,il contient " + std::to_string(nombrePages_) + " et a genéré " + std::to_string(recette_) + "$";
	}

	virtual Item* operator<< (ostream& os) {
		os << afficher();
		return this;
	}
};


class LivreFilm : public Livre, public Film
{
public:
	LivreFilm() = default;

	LivreFilm(const Livre& l, const Film& f) : Item(f.titre_, f.annee_), Livre(l), Film(f) {}

	string afficher() {
		return "";
	}
	Item* operator<< (ostream& os) {
		os << afficher();
		return this;
	}
};