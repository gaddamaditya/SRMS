#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STUD_FILE "students.dat"
#define TKT_FILE "tickets.dat"

// ------------------- STRUCTURES --------------------

typedef struct {
    char id[20];
    char name[50];
    char branch[20];
    char section[10];
    float cgpa;
    char phone[15];
    char password[20];
} Student;

typedef struct {
    int ticketId;
    char studentId[20];  // string ID
    char message[200];
    char status[10];
} Ticket;

// ------------------- BASIC TOOLS --------------------

void clearInput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pauseScreen() {
    printf("\nPress ENTER to continue...");
    clearInput();
}

// ------------------- LOGIN SYSTEM --------------------

int managementLogin() {
    char u[20], p[20];
    printf("----- MANAGEMENT LOGIN -----\n");
    printf("Username: ");
    scanf("%s", u);
    printf("Password: ");
    scanf("%s", p);

    if (strcmp(u, "admin") == 0 && strcmp(p, "admin123") == 0) {
        return 1;
    }
    return 0;
}

int studentLogin(char outId[]) {
    char id[20];
    char pass[20];
    Student s;
    FILE *fp = fopen(STUD_FILE, "rb");
    if (!fp) return 0;

    printf("----- STUDENT LOGIN -----\n");
    printf("Student ID: ");
    scanf("%s", id);
    printf("Password: ");
    scanf("%s", pass);

    while (fread(&s, sizeof(Student), 1, fp)) {
        if (strcmp(s.id, id) == 0 && strcmp(s.password, pass) == 0) {
            fclose(fp);
            strcpy(outId, id);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

// ------------------- MANAGEMENT FUNCTIONS --------------------

void addStudent() {
    Student s;
    FILE *fp = fopen(STUD_FILE, "ab");
    if (!fp) return;

    printf("----- ADD STUDENT -----\n");

    printf("ID: ");
    scanf("%s", s.id);
    clearInput();

    printf("Name: ");
    fgets(s.name, 50, stdin);
    s.name[strcspn(s.name, "\n")] = 0;

    printf("Branch: ");
    fgets(s.branch, 20, stdin);
    s.branch[strcspn(s.branch, "\n")] = 0;

    printf("Section: ");
    fgets(s.section, 10, stdin);
    s.section[strcspn(s.section, "\n")] = 0;

    printf("CGPA: ");
    scanf("%f", &s.cgpa);
    clearInput();

    printf("Phone: ");
    fgets(s.phone, 15, stdin);
    s.phone[strcspn(s.phone, "\n")] = 0;

    sprintf(s.password, "%s@pass", s.id);

    fwrite(&s, sizeof(Student), 1, fp);
    fclose(fp);

    printf("\nStudent added.\n");
    printf("Default password: %s\n", s.password);
    pauseScreen();
}

void displayAll() {
    Student s;
    FILE *fp = fopen(STUD_FILE, "rb");
    if (!fp) {
        printf("No data.\n");
        pauseScreen();
        return;
    }

    printf("\n----- ALL STUDENTS -----\n");

    while (fread(&s, sizeof(Student), 1, fp)) {
        printf("\nID: %s\nName: %s\nBranch: %s\nSection: %s\nCGPA: %.2f\nPhone: %s\n",
               s.id, s.name, s.branch, s.section, s.cgpa, s.phone);
    }

    fclose(fp);
    pauseScreen();
}

void updateStudent() {
    char id[20];
    int found = 0;
    Student s;
    FILE *fp = fopen(STUD_FILE, "rb+");
    if (!fp) return;

    printf("\nEnter ID to update: ");
    scanf("%s", id);
    clearInput();

    while (fread(&s, sizeof(Student), 1, fp)) {
        if (strcmp(s.id, id) == 0) {
            found = 1;

            printf("New Name: ");
            fgets(s.name, 50, stdin);
            s.name[strcspn(s.name, "\n")] = 0;

            printf("New Branch: ");
            fgets(s.branch, 20, stdin);
            s.branch[strcspn(s.branch, "\n")] = 0;

            printf("New Section: ");
            fgets(s.section, 10, stdin);
            s.section[strcspn(s.section, "\n")] = 0;

            printf("New CGPA: ");
            scanf("%f", &s.cgpa);
            clearInput();

            printf("New Phone: ");
            fgets(s.phone, 15, stdin);
            s.phone[strcspn(s.phone, "\n")] = 0;

            fseek(fp, -sizeof(Student), SEEK_CUR);
            fwrite(&s, sizeof(Student), 1, fp);
            break;
        }
    }

    fclose(fp);

    if (found) printf("\nUpdated.\n");
    else printf("\nID not found.\n");

    pauseScreen();
}

void deleteStudent() {
    char id[20];
    int found = 0;
    Student s;
    FILE *fp = fopen(STUD_FILE, "rb");
    FILE *tmp = fopen("tmp.dat", "wb");

    if (!fp || !tmp) return;

    printf("\nEnter ID to delete: ");
    scanf("%s", id);

    while (fread(&s, sizeof(Student), 1, fp)) {
        if (strcmp(s.id, id) == 0) { found = 1; continue; }
        fwrite(&s, sizeof(Student), 1, tmp);
    }

    fclose(fp);
    fclose(tmp);

    remove(STUD_FILE);
    rename("tmp.dat", STUD_FILE);

    if (found) printf("Deleted.\n");
    else printf("ID not found.\n");

    pauseScreen();
}

void changeStudentId() {
    char oldId[20], newId[20];
    int found = 0;
    Student s;
    FILE *fp = fopen(STUD_FILE, "rb+");
    if (!fp) return;

    printf("\nEnter old ID: ");
    scanf("%s", oldId);

    printf("Enter new ID: ");
    scanf("%s", newId);

    while (fread(&s, sizeof(Student), 1, fp)) {
        if (strcmp(s.id, oldId) == 0) {
            strcpy(s.id, newId);
            sprintf(s.password, "%s@pass", s.id);
            fseek(fp, -sizeof(Student), SEEK_CUR);
            fwrite(&s, sizeof(Student), 1, fp);
            found = 1;
            break;
        }
    }

    fclose(fp);

    if (found) printf("\nID changed.\n");
    else printf("\nOld ID not found.\n");

    pauseScreen();
}

// ------------------- TICKET FUNCTIONS --------------------

int nextTicketId() {
    Ticket t;
    int max = 0;
    FILE *fp = fopen(TKT_FILE, "rb");
    if (!fp) return 1;

    while (fread(&t, sizeof(Ticket), 1, fp)) {
        if (t.ticketId > max) max = t.ticketId;
    }

    fclose(fp);
    return max + 1;
}

void raiseTicket(char sid[]) {
    Ticket t;
    FILE *fp = fopen(TKT_FILE, "ab");
    if (!fp) return;

    t.ticketId = nextTicketId();
    strcpy(t.studentId, sid);
    strcpy(t.status, "OPEN");

    printf("\nEnter your concern: ");
    clearInput();
    fgets(t.message, 200, stdin);
    t.message[strcspn(t.message, "\n")] = 0;

    fwrite(&t, sizeof(Ticket), 1, fp);
    fclose(fp);

    printf("\nTicket raised. ID: %d\n", t.ticketId);
    pauseScreen();
}

void viewTickets() {
    Ticket t;
    FILE *fp = fopen(TKT_FILE, "rb");
    if (!fp) {
        printf("No tickets.\n");
        pauseScreen();
        return;
    }

    printf("\n----- ALL TICKETS -----\n");

    while (fread(&t, sizeof(Ticket), 1, fp)) {
        printf("\nTicket ID: %d\nStudent ID: %s\nIssue: %s\nStatus: %s\n",
               t.ticketId, t.studentId, t.message, t.status);
    }

    fclose(fp);
    pauseScreen();
}

void closeTicket() {
    int tid, found = 0;
    Ticket t;
    FILE *fp = fopen(TKT_FILE, "rb");
    FILE *tmp = fopen("tmp_tkt.dat", "wb");

    printf("\nEnter Ticket ID to close: ");
    scanf("%d", &tid);

    while (fread(&t, sizeof(Ticket), 1, fp)) {
        if (t.ticketId == tid) {
            strcpy(t.status, "CLOSED");
            found = 1;
        }
        fwrite(&t, sizeof(Ticket), 1, tmp);
    }

    fclose(fp);
    fclose(tmp);

    remove(TKT_FILE);
    rename("tmp_tkt.dat", TKT_FILE);

    if (found) printf("\nTicket closed.\n");
    else printf("\nTicket not found.\n");

    pauseScreen();
}

// ------------------- STUDENT FUNCTIONS --------------------

void viewMyDetails(char sid[]) {
    Student s;
    FILE *fp = fopen(STUD_FILE, "rb");
    if (!fp) return;

    while (fread(&s, sizeof(Student), 1, fp)) {
        if (strcmp(s.id, sid) == 0) {
            printf("\n----- MY DETAILS -----\n");
            printf("ID: %s\nName: %s\nBranch: %s\nSection: %s\nCGPA: %.2f\nPhone: %s\n",
                   s.id, s.name, s.branch, s.section, s.cgpa, s.phone);
            break;
        }
    }

    fclose(fp);
    pauseScreen();
}

void changePassword(char sid[]) {
    Student s;
    FILE *fp = fopen(STUD_FILE, "rb+");
    if (!fp) return;

    char newPass[20];

    printf("\nEnter new password: ");
    scanf("%s", newPass);

    while (fread(&s, sizeof(Student), 1, fp)) {
        if (strcmp(s.id, sid) == 0) {
            strcpy(s.password, newPass);
            fseek(fp, -sizeof(Student), SEEK_CUR);
            fwrite(&s, sizeof(Student), 1, fp);
            break;
        }
    }

    fclose(fp);

    printf("Password updated.\n");
    pauseScreen();
}

// ------------------- MENUS --------------------

void managementMenu() {
    int c;
    while (1) {
        printf("\n----- MANAGEMENT MENU -----\n");
        printf("1. Add Student\n2. View All Students\n3. Update Student\n4. Delete Student\n5. Change Student ID\n6. View Tickets\n7. Close Ticket\n0. Logout\n");
        printf("Choose: ");
        scanf("%d", &c);

        switch (c) {
            case 1: addStudent(); break;
            case 2: displayAll(); break;
            case 3: updateStudent(); break;
            case 4: deleteStudent(); break;
            case 5: changeStudentId(); break;
            case 6: viewTickets(); break;
            case 7: closeTicket(); break;
            case 0: return;
            default: printf("Invalid.\n"); pauseScreen();
        }
    }
}

void studentMenu(char sid[]) {
    int c;
    while (1) {
        printf("\n----- STUDENT MENU -----\n");
        printf("1. View My Details\n2. Raise Ticket\n3. Change Password\n0. Logout\n");
        printf("Choose: ");
        scanf("%d", &c);

        switch (c) {
            case 1: viewMyDetails(sid); break;
            case 2: raiseTicket(sid); break;
            case 3: changePassword(sid); break;
            case 0: return;
            default: printf("Invalid.\n"); pauseScreen();
        }
    }
}

// ------------------- MAIN --------------------

int main() {
    int ch;
    char sid[20];

    while (1) {
        printf("\n===== STUDENT RECORD MANAGEMENT SYSTEM =====\n");
        printf("1. Management Login\n2. Student Login\n0. Exit\n");
        printf("Choose: ");
        scanf("%d", &ch);

        if (ch == 1) {
            if (managementLogin()) managementMenu();
            else { printf("Login failed.\n"); pauseScreen(); }
        }
        else if (ch == 2) {
            if (studentLogin(sid)) studentMenu(sid);
            else { printf("Login failed.\n"); pauseScreen(); }
        }
        else if (ch == 0) {
            printf("\nExiting...\n");
            return 0;
        }
        else {
            printf("Invalid.\n");
            pauseScreen();
        }
    }

    return 0;
}