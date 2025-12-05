#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STUD_FILE "students.dat"
#define REQ_FILE  "requests.dat"

// ======================= STRUCTURES ==========================
typedef struct {
    char id[20], name[50], branch[20], section[10], phone[15], password[20];
    float cgpa;
} Student;

typedef struct {
    int  requestId;
    char type[10], staffId[20], studentId[20];
} Request;

// ======================= UTILITY ==============================
void clearInput() { int c; while ((c=getchar())!='\n' && c!=EOF); }

// ======================= LOGIN ================================
int adminLogin() {
    char u[20], p[20];
    printf("\n--- ADMIN LOGIN ---\nUsername: "); scanf("%19s", u);
    printf("Password: "); scanf("%19s", p);
    return (!strcmp(u,"admin") && !strcmp(p,"admin123"));
}

int staffLogin(char staffId[]) {
    char u[20], p[20];
    printf("\n--- STAFF LOGIN ---\nStaff ID: "); scanf("%19s", u);
    printf("Password: "); scanf("%19s", p);
    if (!strcmp(u,"staff") && !strcmp(p,"staff123")) { strcpy(staffId,u); return 1; }
    return 0;
}

int studentLogin(char studentId[]) {
    Student s; char id[20], pass[20];
    FILE *fp=fopen(STUD_FILE,"rb");
    if(!fp){ printf("No student DB.\n"); return 0; }
    printf("\n--- STUDENT LOGIN ---\nID: "); scanf("%19s",id);
    printf("Password: "); scanf("%19s",pass);
    while(fread(&s,sizeof(Student),1,fp))
        if(!strcmp(s.id,id)&&!strcmp(s.password,pass)){ fclose(fp); strcpy(studentId,id); return 1;}
    fclose(fp); return 0;
}

// ======================= ADMIN CRUD ===========================
void addStudent() {
    Student s; FILE *fp=fopen(STUD_FILE,"ab");
    if(!fp){printf("File error.\n");return;}
    printf("\n--- ADD STUDENT ---\nID: "); scanf("%19s", s.id);
    clearInput(); printf("Name: "); fgets(s.name,50,stdin); s.name[strcspn(s.name,"\n")]=0;
    printf("Branch: "); fgets(s.branch,20,stdin); s.branch[strcspn(s.branch,"\n")]=0;
    printf("Section: "); fgets(s.section,10,stdin); s.section[strcspn(s.section,"\n")]=0;
    printf("CGPA: "); scanf("%f",&s.cgpa); clearInput();
    printf("Phone: "); fgets(s.phone,15,stdin); s.phone[strcspn(s.phone,"\n")]=0;
    sprintf(s.password,"%s@pass",s.id);
    fwrite(&s,sizeof(Student),1,fp); fclose(fp);
    printf("Student added. Default password = %s\n",s.password);
}

void viewStudents() {
    Student s; FILE *fp=fopen(STUD_FILE,"rb");
    if(!fp){ printf("No records.\n"); return; }
    printf("\n--- STUDENTS ---\n");
    while(fread(&s,sizeof(Student),1,fp)){
        printf("\nID:%s\nName:%s\nBranch:%s\nSec:%s\nCGPA:%.2f\nPhone:%s\n",
               s.id,s.name,s.branch,s.section,s.cgpa,s.phone);
    }
    fclose(fp);
}

void searchStudent() {
    char id[20]; Student s; int f=0;
    printf("Enter ID: "); scanf("%19s",id);
    FILE *fp=fopen(STUD_FILE,"rb");
    if(!fp){ printf("No records.\n"); return; }
    while(fread(&s,sizeof(Student),1,fp)){
        if(!strcmp(s.id,id)){
            printf("\nID:%s\nName:%s\nBranch:%s\nSec:%s\nCGPA:%.2f\nPhone:%s\n",
                   s.id,s.name,s.branch,s.section,s.cgpa,s.phone);
            f=1; break;
        }
    }
    fclose(fp);
    if(!f) printf("Not found.\n");
}

void updateStudent() {
    char id[20]; Student s; int found=0;
    printf("Enter ID: "); scanf("%19s",id);
    FILE *fp=fopen(STUD_FILE,"rb+");
    if(!fp){ printf("No records.\n"); return; }
    while(fread(&s,sizeof(Student),1,fp)){
        if(!strcmp(s.id,id)){
            found=1; clearInput();
            printf("New Name: "); fgets(s.name,50,stdin); s.name[strcspn(s.name,"\n")] = 0;
            printf("New Branch: "); fgets(s.branch,20,stdin); s.branch[strcspn(s.branch,"\n")] = 0;
            printf("New Section: "); fgets(s.section,10,stdin); s.section[strcspn(s.section,"\n")] = 0;
            printf("New CGPA: "); scanf("%f",&s.cgpa); clearInput();
            printf("New Phone: "); fgets(s.phone,15,stdin); s.phone[strcspn(s.phone,"\n")] = 0;
            fseek(fp,-sizeof(Student),SEEK_CUR); fwrite(&s,sizeof(Student),1,fp);
            printf("Updated.\n"); fclose(fp); return;
        }
    }
    fclose(fp);
    printf("Not found.\n");
}

void deleteStudent() {
    char id[20]; Student s; int found=0;
    printf("Enter ID to delete: "); scanf("%19s",id);
    FILE *fp=fopen(STUD_FILE,"rb"), *tmp=fopen("tmp.dat","wb");
    if(!fp||!tmp){ printf("File error.\n"); if(fp)fclose(fp); return; }
    while(fread(&s,sizeof(Student),1,fp)){
        if(!strcmp(s.id,id)) found=1;
        else fwrite(&s,sizeof(Student),1,tmp);
    }
    fclose(fp); fclose(tmp);
    remove(STUD_FILE); rename("tmp.dat",STUD_FILE);
    printf(found?"Deleted.\n":"ID not found.\n");
}

// ====================== STUDENT VIEW ==========================
void viewMyDetails(char sid[]) {
    Student s; FILE *fp=fopen(STUD_FILE,"rb");
    if(!fp){ printf("No records.\n"); return; }
    while(fread(&s,sizeof(Student),1,fp)){
        if(!strcmp(s.id,sid)){
            printf("\nID:%s\nName:%s\nBranch:%s\nSec:%s\nCGPA:%.2f\nPhone:%s\n",
                   s.id,s.name,s.branch,s.section,s.cgpa,s.phone);
            fclose(fp); return;
        }
    }
    fclose(fp); printf("Record not found.\n");
}

// ======================= REQUEST SYSTEM =======================
int nextRequestId() {
    Request r; int max=0; FILE *fp=fopen(REQ_FILE,"rb");
    if(!fp) return 1;
    while(fread(&r,sizeof(Request),1,fp)) if(r.requestId>max) max=r.requestId;
    fclose(fp); return max+1;
}

void requestAdd(char sid[]) {
    Request r; FILE *fp=fopen(REQ_FILE,"ab"); if(!fp){printf("Err.\n");return;}
    r.requestId=nextRequestId(); strcpy(r.type,"ADD"); strcpy(r.staffId,sid);
    printf("Enter Student ID: "); scanf("%19s",r.studentId);
    fwrite(&r,sizeof(Request),1,fp); fclose(fp);
    printf("Request submitted (%d)\n",r.requestId);
}

void requestUpdate(char sid[]) {
    Request r; FILE *fp=fopen(REQ_FILE,"ab"); if(!fp){printf("Err.\n");return;}
    r.requestId=nextRequestId(); strcpy(r.type,"UPDATE"); strcpy(r.staffId,sid);
    printf("Enter Student ID: "); scanf("%19s",r.studentId);
    fwrite(&r,sizeof(Request),1,fp); fclose(fp);
    printf("Request submitted (%d)\n",r.requestId);
}

void requestDelete(char sid[]) {
    Request r; FILE *fp=fopen(REQ_FILE,"ab"); if(!fp){printf("Err.\n");return;}
    r.requestId=nextRequestId(); strcpy(r.type,"DELETE"); strcpy(r.staffId,sid);
    printf("Enter Student ID: "); scanf("%19s",r.studentId);
    fwrite(&r,sizeof(Request),1,fp); fclose(fp);
    printf("Request submitted (%d)\n",r.requestId);
}

void viewRequests() {
    Request r; FILE *fp=fopen(REQ_FILE,"rb");
    if(!fp){ printf("No requests.\n"); return; }
    printf("\n--- REQUESTS ---\n");
    while(fread(&r,sizeof(Request),1,fp))
        printf("\nID:%d\nType:%s\nStaff:%s\nStudent:%s\n",
               r.requestId,r.type,r.staffId,r.studentId);
    fclose(fp);
}

// ======================= APPROVALS ============================
void approveAdd() {
    int id,found=0; Request r;
    printf("Enter Request ID: "); scanf("%d",&id);
    FILE *fp=fopen(REQ_FILE,"rb"), *tmp=fopen("rtmp.dat","wb");
    if(!fp||!tmp){ printf("Err.\n"); return; }
    while(fread(&r,sizeof(Request),1,fp)){
        if(r.requestId==id&&!strcmp(r.type,"ADD")){
            found=1;
            Student s; strcpy(s.id,r.studentId);
            clearInput();
            printf("Name: "); fgets(s.name,50,stdin); s.name[strcspn(s.name,"\n")] = 0;
            printf("Branch: "); fgets(s.branch,20,stdin); s.branch[strcspn(s.branch,"\n")] = 0;
            printf("Section: "); fgets(s.section,10,stdin); s.section[strcspn(s.section,"\n")] = 0;
            printf("CGPA: "); scanf("%f",&s.cgpa); clearInput();
            printf("Phone: "); fgets(s.phone,15,stdin); s.phone[strcspn(s.phone,"\n")] = 0;
            sprintf(s.password,"%s@pass",s.id);
            FILE *sf=fopen(STUD_FILE,"ab"); fwrite(&s,sizeof(Student),1,sf); fclose(sf);
            printf("Added.\n"); continue;
        }
        fwrite(&r,sizeof(Request),1,tmp);
    }
    fclose(fp); fclose(tmp);
    remove(REQ_FILE); rename("rtmp.dat",REQ_FILE);
    if(!found) printf("Not found.\n");
}

void approveUpdate() {
    int id,found=0; Request r;
    printf("Enter Request ID: "); scanf("%d",&id);
    FILE *fp=fopen(REQ_FILE,"rb"), *tmp=fopen("rtmp.dat","wb");
    if(!fp||!tmp){ printf("Err.\n"); return; }

    while(fread(&r,sizeof(Request),1,fp)){
        if(r.requestId==id&&!strcmp(r.type,"UPDATE")){
            found=1;
            Student s; int updated=0;
            FILE *sf=fopen(STUD_FILE,"rb"), *st=fopen("stmp.dat","wb");
            while(fread(&s,sizeof(Student),1,sf)){
                if(!strcmp(s.id,r.studentId)){
                    updated=1; clearInput();
                    printf("New Name: "); fgets(s.name,50,stdin); s.name[strcspn(s.name,"\n")] = 0;
                    printf("New Branch: "); fgets(s.branch,20,stdin); s.branch[strcspn(s.branch,"\n")] = 0;
                    printf("New Section: "); fgets(s.section,10,stdin); s.section[strcspn(s.section,"\n")] = 0;
                    printf("New CGPA: "); scanf("%f",&s.cgpa); clearInput();
                    printf("New Phone: "); fgets(s.phone,15,stdin); s.phone[strcspn(s.phone,"\n")] = 0;
                }
                fwrite(&s,sizeof(Student),1,st);
            }
            fclose(sf); fclose(st);
            remove(STUD_FILE); rename("stmp.dat",STUD_FILE);
            printf(updated?"Updated.\n":"Student not found.\n");
            continue;
        }
        fwrite(&r,sizeof(Request),1,tmp);
    }
    fclose(fp); fclose(tmp);
    remove(REQ_FILE); rename("rtmp.dat",REQ_FILE);
    if(!found) printf("Not found.\n");
}

void approveDelete() {
    int id,found=0; Request r;
    printf("Enter Request ID: "); scanf("%d",&id);
    FILE *fp=fopen(REQ_FILE,"rb"), *tmp=fopen("rtmp.dat","wb");
    if(!fp||!tmp){ printf("Err.\n"); return; }

    while(fread(&r,sizeof(Request),1,fp)){
        if(r.requestId==id&&!strcmp(r.type,"DELETE")){
            found=1;
            Student s; int deleted=0;
            FILE *sf=fopen(STUD_FILE,"rb"), *st=fopen("stmp.dat","wb");
            while(fread(&s,sizeof(Student),1,sf)){
                if(!strcmp(s.id,r.studentId)) deleted=1;
                else fwrite(&s,sizeof(Student),1,st);
            }
            fclose(sf); fclose(st);
            remove(STUD_FILE); rename("stmp.dat",STUD_FILE);
            printf(deleted?"Deleted.\n":"Not found.\n");
            continue;
        }
        fwrite(&r,sizeof(Request),1,tmp);
    }
    fclose(fp); fclose(tmp);
    remove(REQ_FILE); rename("rtmp.dat",REQ_FILE);
    if(!found) printf("Not found.\n");
}

// ======================= MENUS ================================
void adminMenu() {
    int ch;
    while(1){
        printf("\n1.Add\n2.View\n3.Search\n4.Update\n5.Delete\n6.Show Requests\n7.Approve Add\n8.Approve Update\n9.Approve Delete\n0.Logout\nChoose: ");
        scanf("%d",&ch);
        switch(ch){
            case 1:addStudent();break;
            case 2:viewStudents();break;
            case 3:searchStudent();break;
            case 4:updateStudent();break;
            case 5:deleteStudent();break;
            case 6:viewRequests();break;
            case 7:approveAdd();break;
            case 8:approveUpdate();break;
            case 9:approveDelete();break;
            case 0:return;
            default:printf("Invalid\n");
        }
    }
}

void staffMenu(char sid[]) {
    int ch;
    while(1){
        printf("\n1.View Students\n2.Search\n3.Request Add\n4.Request Update\n5.Request Delete\n0.Logout\nChoose: ");
        scanf("%d",&ch);
        switch(ch){
            case 1:viewStudents();break;
            case 2:searchStudent();break;
            case 3:requestAdd(sid);break;
            case 4:requestUpdate(sid);break;
            case 5:requestDelete(sid);break;
            case 0:return;
            default:printf("Invalid\n");
        }
    }
}

void studentMenu(char sid[]) {
    int ch;
    while(1){
        printf("\n1.View My Details\n0.Logout\nChoose: ");
        scanf("%d",&ch);
        if(ch==1) viewMyDetails(sid);
        else if(ch==0) return;
        else printf("Invalid\n");
    }
}

// ======================= MAIN ================================
int main() {
    int ch; char sid[20], stid[20];
    while(1){
        printf("\n1.Admin\n2.Staff\n3.Student\n0.Exit\nChoose: ");
        scanf("%d",&ch);
        if(ch==1){ if(adminLogin()) adminMenu(); else printf("Invalid.\n"); }
        else if(ch==2){ if(staffLogin(sid)) staffMenu(sid); else printf("Invalid.\n"); }
        else if(ch==3){ if(studentLogin(stid)) studentMenu(stid); else printf("Invalid.\n"); }
        else if(ch==0){ printf("Bye!\n"); break; }
        else printf("Invalid.\n");
    }
    return 0;
}
