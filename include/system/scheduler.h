#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <system/exceptions.h>

/**
 * \file scheduler.h
 * \brief Funktionen zum verwalten des Schedulers und starten einzelner Threads.
 *
Insgesamt gibt es die Möglichkeit 32 Threads parallel aktiv zu verwalten
 * Der Idle Thread ist hierbei separat und hat immer die id -1. Der Scheduler
 * muss zuerst mit \p scheduler_init() initialisiert werden.
 */

/* Anzahl der verfügbaren Threads.*/
#define MAX_THREADS (32)
/* Idle Thread id.*/
#define IDLE_THREAD_ID (-1)

/**
 * \brief Alle States in denen sich ein Thred befinden kann
 */
enum thread_status {
    FINISHED, ///< Thread ist beendet
    READY,    ///< Thread ist bereit
    RUNNING,  ///< Thread is momentan aktiv
    IS_IDLE,  ///< Idle Thread
    OTHER,    ///< Extern verwaltet
    ERROR     ///< Fehler
};

/* Errors */
#define ERR_TCB_FULL      ((int) -1)
#define ERR_ARGS_SIZE     ((int) -2)
#define ERR_INVALID_ID    ((int) -3)
#define ERR_INVALID_STATE ((int) -4)


/**
 * \brief Initialisiert den Scheduler
 *
 * Sicher stellen, dass unterhalb von user_stack_begin insgesamt
 * user_stack_size * 32 Platzt ist (falls user_same_stack false),
 * da dort die Stacks für die user threads angelegt werden.
 * Der initiale Thread ist der Idle Thread.
 *
 * \param user_stack_begin Adresse wo user stacks beginnen (höchste addr).
 * \param user_stack_size Größe des Stacks für einen user thread.
 * \param user_same_stacks Falls true erhalten alle neuen Threads den
 *                         selben Stack zugewiesen.
 */
void scheduler_init(
        unsigned int user_stack_begin,
        unsigned int user_stack_size,
        unsigned int user_same_stacks);

/**
 * \brief Git die id des momentan aktiven Thread aus
 * \return Id des aktuellen aktiven Threads
 */
int scheduler_get_current_thread();

/**
 * \brief Gibt die momentan im tcb abgespeicherten Register des
 *        Threads mit der angeforderten id zurück
 *
 * \param id Id des Threads
 * \return Struct mit allen gespeicherten Registerwerten
 *         NULL falls invalide id
 */
struct regs *scheduler_get_regs(int id);

/**
 * \brief Gibt den Status des Threads mit der angeforderten id zurück
 *
 * \param id Id des Threads.
 * \return Thread Status.
 *         ERROR bei invalider id.
 */
enum thread_status scheduler_get_state(int id);

/**
 * \brief Gibt die Stack Basis des TCB mit der angeforderten id zurück.
 *
 * \param id Id des Threads
 * \return Stack Basis
 */
unsigned int scheduler_get_stack_base(int id);

/**
 * \brief Round Robin, alter Thread wird auf READY gesetzt
 *
 * Die Register auf dem Stack werden im TCB des jeweiligen Threads
 * abgespeichert und die Registerwerte des nächsten Threads aus dem TCB
 * auf den Stack geladen.
 * Dabei wird der alte Thread wieder zurück in die ready Warteschlange gelegt.
 *
 * \param regs Registerkontext des aktuellen Threads.
 * \return Id des nächsten Threads.
 */
int scheduler_round_robin(struct regs *regs);

/**
 * \brief Round Robin, alter Thread wird auf OTHER gesetzt
 *
 * Wie scheduler_round_robin, aber der alte Thread wird nicht zurück in
 * die ready Warteschlange geladen und bekommt den Status: OTHER.
 * Der Thread kann dann manuell verwaltet werden. Um den Thread wieder zurück
 * in die ready Warteschlange zu schieben können die Funktionen
 * scheduler_add_to_ready_[first,last] verwendet werden
 *
 * Falls der momentan aktive Thread bereits beendet wurde, verhält sich diese
 * Funktion wie scheduler_round_robin
 *
 * \param regs Registerkontext des aktuellen Threads.
 * \return Id des nächsten Threads.
 */
int scheduler_next(struct regs *regs);

/**
 * \brief Initialisiert einen neuen Thread und fügt diesen in die
 *        ready Warteschlange hinzu.
 *
 * \param func Start Adresse des neuen Thrads
 * \param arg Pointer zu den Argumenten. Diese werden auf den Stack
 *            geladen und r0 des Threads passend gesetzt
 * \param arg_size Größe der Daten zu den arg zeigt.
 * \return Neue id des angelegten Threads.
 *         ERR_TCB_FULL falls alle Threads belegt.
 *         ERR_ARGS_SIZE falls Argumente zu groß.
 *
 */
int scheduler_new_thread(void (*func)(void *), const void *arg, unsigned int arg_size);

/**
 * \brief Setzt den Status des momentanen Threads auf FINISHED.
 *
 * Schlägt fehl, wenn der aktuelle Thread der Idle Thread ist.
 * Um den Kontextwechsel zum nächsten Thread durchzuführen, muss anschließend
 * scheduler_round_robin aufgerufen werden
 * \return 0 falls erfolgreich.
 *         ERR_INVALID_ID
 */
int scheduler_end_current(void);

/**
 * \brief Fügt einen Thread wieder in die interne Liste hinzu
 *
 * Folgende Funktionen fügen einen Thread wieder in die intern verwaltete
 * ready Warteschlange hinzu. Funktioniert nur wenn sich der Thread im
 * OTHER state befindet. Bei Erfolgreichem Einfügen, wird der state des
 * Threads entsprechend angepasst. Der Thread wird am Anfang der
 * Warteschlange eingefügt.
 *
 * \param id Id des Threads.
 * \return 0 falls erfolgreich.
 *         ERR_INVALID_ID bei invalider id.
 *         ERR_INVALID_STATE falls Thread nicht im OTHER state.
 */
int scheduler_add_to_ready_last(int id);

/**
 * \brief Fügt einen Thread wieder in die interne Liste hinzu
 *
 * Wie \ref scheduler_add_to_ready_last nur das der Thread am Anfang der
 * ready Warteschlange eingefügt wird
 */
int scheduler_add_to_ready_first(int id);


/**
 * \brief Hook für Verwaltung, etc...
 *
 * Diese Funktion wird bei einem Kontextwechsel aufgerufen nachdem die
 * die Register auf dem Stack ausgetauscht wurden.
 * Die Funktion ist intern als __attribute__((weak)) definiert und macht nichts.
 * Folglich kann diese Funktion überschrieben werden.
 * Mögliche Anwendungen:
 * - Debugging
 * - Verwaltung
 *
 * \param old Id des vorherigen Threads
 * \param next Id des nächsten Threads
 */
extern void scheduler_on_schedule(int old, int next);

/**
 * \brief Soll den Syscall zum beenden eines Threads aufrufen
 *
 * Diese Funktion muss selbst implementiert werden
 * Diese Funktion wird beim erstellen eines threads in das
 * lr Register geschrieben und soll den Thread vernünftig
 * beenden (per syscall!)
 * */
extern void scheduler_thread_terminate(void);

#endif
