#include <algorithm>
#include <chrono>
#include <cctype>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace {
std::string trim(std::string s) {
    const auto first = s.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    const auto last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, last - first + 1);
}

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    return trim(value);
}

int readInt(const std::string& prompt, int minValue, int maxValue) {
    while (true) {
        const std::string raw = readLine(prompt);
        try {
            std::size_t used = 0;
            const int value = std::stoi(raw, &used);
            if (used == raw.size() && value >= minValue && value <= maxValue) return value;
        } catch (...) {}
        std::cout << "Entrée invalide. Choisissez un nombre entre " << minValue << " et " << maxValue << ".\n";
    }
}

std::string now() {
    const auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &time);
#else
    localtime_r(&time, &local);
#endif
    std::ostringstream out;
    out << std::put_time(&local, "%Y-%m-%d %H:%M");
    return out.str();
}

std::string money(double amount) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(0) << amount << " FCFA";
    return out.str();
}

enum class ReservationStatus { EnAttente, Confirmee, Annulee };
std::string statusName(ReservationStatus status) {
    switch (status) {
        case ReservationStatus::EnAttente: return "En attente de paiement";
        case ReservationStatus::Confirmee: return "Confirmée";
        case ReservationStatus::Annulee: return "Annulée";
    }
    return "Inconnue";
}

struct Passenger {
    int id{};
    std::string name, phone, email, passport;
};

struct Flight {
    int id{};
    std::string code, origin, destination, departure;
    int capacity{};
    double price{};
    bool active{true};
};

struct Reservation {
    int id{}, passengerId{}, flightId{};
    ReservationStatus status{ReservationStatus::EnAttente};
    std::string createdAt;
    std::optional<std::string> paidAt;
};

struct Payment {
    int reservationId{};
    double amount{};
    std::string method;
    std::string paidAt;
};

class TravelAgency {
    std::map<int, Passenger> passengers;
    std::map<int, Flight> flights;
    std::map<int, Reservation> reservations;
    std::map<int, Payment> payments;
    int nextPassenger{1}, nextFlight{1}, nextReservation{1};

    int occupiedSeats(int flightId) const {
        int count = 0;
        for (const auto& [id, reservation] : reservations) {
            (void)id;
            if (reservation.flightId == flightId && reservation.status == ReservationStatus::Confirmee) ++count;
        }
        return count;
    }

    const Passenger* getPassenger(int id) const {
        auto it = passengers.find(id);
        return it == passengers.end() ? nullptr : &it->second;
    }
    const Flight* getFlight(int id) const {
        auto it = flights.find(id);
        return it == flights.end() ? nullptr : &it->second;
    }

    void showFlight(const Flight& f) const {
        std::cout << "Vol #" << f.id << " | " << f.code << " | " << f.origin << " -> " << f.destination
                  << " | Départ : " << f.departure << " | Prix : " << money(f.price)
                  << " | Places disponibles : " << std::max(0, f.capacity - occupiedSeats(f.id)) << '/' << f.capacity
                  << (f.active ? " | Ouvert" : " | Fermé") << '\n';
    }

    const Reservation* chooseReservation(const std::string& prompt) const {
        if (reservations.empty()) {
            std::cout << "Aucune réservation enregistrée.\n";
            return nullptr;
        }
        const int id = readInt(prompt, 1, std::numeric_limits<int>::max());
        auto it = reservations.find(id);
        if (it == reservations.end()) {
            std::cout << "Réservation introuvable.\n";
            return nullptr;
        }
        return &it->second;
    }

    void showReservation(const Reservation& r) const {
        const Passenger* p = getPassenger(r.passengerId);
        const Flight* f = getFlight(r.flightId);
        std::cout << "Réservation #" << r.id << " | Statut : " << statusName(r.status) << "\n"
                  << "  Passager : " << (p ? p->name : "inconnu") << " (ID " << r.passengerId << ")\n"
                  << "  Vol : " << (f ? f->code + " " + f->origin + " -> " + f->destination : "inconnu")
                  << " (ID " << r.flightId << ")\n  Créée le : " << r.createdAt;
        if (r.paidAt) std::cout << " | Payée le : " << *r.paidAt;
        std::cout << '\n';
    }

public:
    void addPassenger() {
        Passenger p;
        p.id = nextPassenger++;
        p.name = readLine("Nom complet : ");
        p.phone = readLine("Téléphone : ");
        p.email = readLine("E-mail (facultatif) : ");
        p.passport = readLine("Numéro de passeport/pièce : ");
        if (p.name.empty() || p.phone.empty() || p.passport.empty()) {
            --nextPassenger;
            std::cout << "Nom, téléphone et pièce d'identité sont obligatoires. Aucun passager ajouté.\n";
            return;
        }
        passengers.emplace(p.id, p);
        std::cout << "Passager enregistré avec l'ID " << p.id << ".\n";
    }

    void addFlight() {
        Flight f;
        f.id = nextFlight++;
        f.code = readLine("Code du vol : ");
        f.origin = readLine("Ville de départ : ");
        f.destination = readLine("Destination : ");
        f.departure = readLine("Date et heure de départ (ex. 2026-12-20 08:30) : ");
        f.capacity = readInt("Capacité (places) : ", 1, 100000);
        f.price = readInt("Prix en FCFA : ", 1, std::numeric_limits<int>::max());
        if (f.code.empty() || f.origin.empty() || f.destination.empty() || f.departure.empty()) {
            --nextFlight;
            std::cout << "Tous les champs du vol sont obligatoires. Aucun vol ajouté.\n";
            return;
        }
        for (const auto& [id, existing] : flights) {
            (void)id;
            if (existing.code == f.code) {
                --nextFlight;
                std::cout << "Ce code de vol existe déjà.\n";
                return;
            }
        }
        flights.emplace(f.id, f);
        std::cout << "Vol ajouté avec l'ID " << f.id << ".\n";
    }

    void listFlights(bool onlyAvailable) const {
        bool found = false;
        for (const auto& [id, flight] : flights) {
            (void)id;
            if (!flight.active || (onlyAvailable && occupiedSeats(flight.id) >= flight.capacity)) continue;
            showFlight(flight);
            found = true;
        }
        if (!found) std::cout << "Aucun vol " << (onlyAvailable ? "disponible" : "enregistré") << ".\n";
    }

    void searchFlight() const {
        const std::string query = readLine("Ville/code à rechercher : ");
        if (query.empty()) { std::cout << "Recherche vide.\n"; return; }
        auto lower = [](std::string s) { std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); }); return s; };
        const std::string needle = lower(query);
        bool found = false;
        for (const auto& [id, f] : flights) {
            (void)id;
            if (!f.active) continue;
            if (lower(f.code + " " + f.origin + " " + f.destination + " " + f.departure).find(needle) != std::string::npos) {
                showFlight(f); found = true;
            }
        }
        if (!found) std::cout << "Aucun vol ne correspond à cette recherche.\n";
    }

    void reserve() {
        if (passengers.empty()) { std::cout << "Enregistrez d'abord un passager.\n"; return; }
        if (flights.empty()) { std::cout << "Ajoutez d'abord un vol.\n"; return; }
        const int passengerId = readInt("ID du passager : ", 1, std::numeric_limits<int>::max());
        if (!getPassenger(passengerId)) { std::cout << "Passager introuvable.\n"; return; }
        listFlights(true);
        const int flightId = readInt("ID du vol choisi : ", 1, std::numeric_limits<int>::max());
        const Flight* f = getFlight(flightId);
        if (!f || !f->active) { std::cout << "Vol introuvable ou fermé.\n"; return; }
        if (occupiedSeats(flightId) >= f->capacity) { std::cout << "Vol complet : capacité maximale atteinte.\n"; return; }
        for (const auto& [id, r] : reservations) {
            (void)id;
            if (r.passengerId == passengerId && r.flightId == flightId && r.status != ReservationStatus::Annulee) {
                std::cout << "Ce passager possède déjà une réservation active pour ce vol (ID " << r.id << ").\n";
                return;
            }
        }
        Reservation r{nextReservation++, passengerId, flightId, ReservationStatus::EnAttente, now(), std::nullopt};
        reservations.emplace(r.id, r);
        std::cout << "Réservation #" << r.id << " créée en attente de paiement. La place sera confirmée après validation du paiement.\n";
    }

    void pay() {
        const Reservation* selected = chooseReservation("ID de la réservation à payer : ");
        if (!selected) return;
        auto it = reservations.find(selected->id);
        Reservation& r = it->second;
        if (r.status == ReservationStatus::Annulee) { std::cout << "Paiement refusé : la réservation est annulée.\n"; return; }
        if (r.status == ReservationStatus::Confirmee) { std::cout << "Cette réservation est déjà payée et confirmée.\n"; return; }
        const Flight* f = getFlight(r.flightId);
        if (!f) { std::cout << "Vol introuvable ; paiement impossible.\n"; return; }
        if (occupiedSeats(r.flightId) >= f->capacity) { std::cout << "Paiement refusé : la capacité du vol est atteinte.\n"; return; }
        std::cout << "Montant à percevoir : " << money(f->price) << '\n';
        const std::string method = readLine("Mode de paiement (espèces, mobile money, carte) : ");
        if (method.empty()) { std::cout << "Mode de paiement obligatoire. Paiement non enregistré.\n"; return; }
        r.status = ReservationStatus::Confirmee;
        r.paidAt = now();
        payments.emplace(r.id, Payment{r.id, f->price, method, *r.paidAt});
        std::cout << "Paiement enregistré (" << method << "). Réservation confirmée.\n";
    }

    void cancel() {
        const Reservation* selected = chooseReservation("ID de la réservation à annuler : ");
        if (!selected) return;
        Reservation& r = reservations.at(selected->id);
        if (r.status == ReservationStatus::Annulee) { std::cout << "Cette réservation est déjà annulée.\n"; return; }
        r.status = ReservationStatus::Annulee;
        std::cout << "Réservation #" << r.id << " annulée. La place est libérée. Traitez tout remboursement selon les règles de l'agence.\n";
    }

    void listReservations() const {
        if (reservations.empty()) { std::cout << "Aucune réservation.\n"; return; }
        for (const auto& [id, r] : reservations) { (void)id; showReservation(r); }
    }

    void reservationInfo() const {
        if (reservations.empty()) { std::cout << "Aucune réservation.\n"; return; }
        const int id = readInt("ID de la réservation : ", 1, std::numeric_limits<int>::max());
        auto it = reservations.find(id);
        if (it == reservations.end()) { std::cout << "Réservation introuvable.\n"; return; }
        showReservation(it->second);
        const Flight* f = getFlight(it->second.flightId);
        if (f) std::cout << "  Tarif : " << money(f->price) << " | Départ : " << f->departure << '\n';
    }
};

void menu() {
    std::cout << "\n========== AGENCE DE VOYAGES DE COTONOU ==========\n"
              << "1. Enregistrer un passager\n2. Ajouter un vol\n3. Afficher les vols disponibles\n"
              << "4. Rechercher un vol\n5. Effectuer une réservation\n6. Enregistrer un paiement\n"
              << "7. Annuler une réservation\n8. Consulter les réservations\n"
              << "9. Afficher les informations d'une réservation\n10. Quitter\n";
}
} // namespace

int main() {
    TravelAgency agency;
    std::cout << "Gestion des réservations de billets d'avion — Cotonou\n"
              << "Note : les données restent en mémoire et sont perdues à la fermeture.\n";
    bool running = true;
    while (running && std::cin) {
        menu();
        switch (readInt("Votre choix : ", 1, 10)) {
            case 1: agency.addPassenger(); break;
            case 2: agency.addFlight(); break;
            case 3: agency.listFlights(true); break;
            case 4: agency.searchFlight(); break;
            case 5: agency.reserve(); break;
            case 6: agency.pay(); break;
            case 7: agency.cancel(); break;
            case 8: agency.listReservations(); break;
            case 9: agency.reservationInfo(); break;
            case 10: running = false; std::cout << "Au revoir.\n"; break;
        }
    }
    return 0;
}
