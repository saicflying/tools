
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct _p_v {
        int max;  
        int curtry;
        unsigned long possible_data;
};

static int matrix[9][9];
static struct _p_v  pv[9][9];
static int gl_max = 0;


void find_min_pos(int *x, int *y, struct _p_v lpv[][9]);
int possible_value(int lm[][9], int i, int j, struct _p_v lpv[][9], unsigned long pos_val);

void show_matrix(int lmt[][9])
{
        int i, j;
        printf("............................\n");
        for (i = 0; i < 9; i++) {
                for (j = 0; j < 9; j++) {
                        printf("%d ", lmt[i][j]);
                        if (j == 8) {
                                printf("\n"); 
                        }
                }
        }
        printf("............................\n");

}

char * data_to_string(unsigned long data)
{
   int i;
   static char string[10];
   for (i = 8; i >=0; i--) {
    string[8 - i] = (data & (1 << i))?'1':'0';
   }     
   string[9] = '\0';
   return string;
}

int solve_matrix(int lmt[][9], struct _p_v lpv[][9])
{
    int i, j = 0, k, m = 0, n = 0;
    int new_matrix[9][9];
    struct _p_v  new_pv[9][9];
    int x, y, ret;
    int pos_val;

retry:
    m = 0;
    n = 0;
    show_matrix(lmt);
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            if (lmt[i][j]) {
                m++;
            }
        }
    }
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
          if (lpv[i][j].max == 0) {
                pos_val = 0xffff; 
          } else {
                pos_val = lpv[i][j].possible_data;
          }
          if (possible_value(lmt, i, j, lpv, pos_val) < 0)
          {
                //printf("failed1\n");
                return -1;
          }
          if (lpv[i][j].max == 1) {
                  for ( k = 0; k < 9; k++) {
                          if (lpv[i][j].possible_data & ( 1 << k)) {
                                lmt[i][j] = k + 1;
                                break;
                          }
                  }
          }
        }
    } 
    gc_matrix_1(lmt, lpv);
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
          if (lpv[i][j].max == 1) {
              n++; 
          }
        }
    } 

    //printf(" m = %d, n = %d\n", m, n);
    //getchar();
    if (m == n) {
            
    } else {
        goto retry;   
    }

    for (i = 0; i < 9; i++) {
            for (j = 0; j < 9; j++) {
                    if (lpv[i][j].max > 1) {                       
                       find_min_pos(&x, &y, lpv);               
                       if (x >= 0 && y >= 0) {
                               for (k = 0; k < 9; k++) {
                                       if (lpv[x][y].possible_data & (1 << k)) {
                                               printf("try possible value at %d, %d (%d in %s(%d))\n", x, y, k + 1, data_to_string(lpv[x][y].possible_data), lpv[x][y].max);
                                               memcpy(&new_matrix, lmt, sizeof(new_matrix)); 
                                               memcpy(&new_pv, lpv, sizeof(new_pv)); 
                                               new_matrix[x][y] = k + 1;
                                               ret = solve_matrix(new_matrix, new_pv); 
                                               if (ret < 0) {   
                                                    continue;
                                               } else if (ret == 1) {
                                                   memcpy(lmt, &new_matrix,sizeof(new_matrix));
                                                   return 1;
                                               }
                                       }
                               } 
                               if (k == 9) {
                                   // printf("failed2\n");
                                    return -1;
                               }
                       } 
                       return 0; 
                    }
            }
    }

    return  1;
}

int gc_matrix_1(int lm[][9], struct _p_v lpv[][9])
{   
    int i, j, v, c, dx, dy,m,n;
    for ( i = 0; i < 9; i++) {
        for (v = 1; v < 10; v++) {
                c = 0;
                dx = -1;
                for (j = 0; j < 9; j++) {
                        if (lpv[i][j].possible_data & ( 1 << (v - 1))) {
                                c++;
                                dx = j;
                        } 
                }
                if (c == 1) {
                    lm[i][dx] = v;
                    lpv[i][dx].max = 1;
                    lpv[i][dx].possible_data = 1 << (v - 1);
                }
        } 
    }

    for ( i = 0; i < 9; i++) {
        for (v = 1; v < 10; v++) {
                c = 0;
                dx = -1;
                for (j = 0; j < 9; j++) {
                        if (lpv[j][i].possible_data & ( 1 << (v - 1))) {
                                c++;
                                dx = j;
                        } 
                }
                if (c == 1) {
                    lm[dx][i] = v;
                    lpv[dx][i].max = 1;
                    lpv[dx][i].possible_data = 1 << (v - 1);
                }
        } 
    } 

    for (i = 0; i < 9; i++) {
        for (v = 1; v < 10; v++) {
           for (m = 0; m < 3; m++) {
                for (n = 0; n < 3; n++) {
                    if (lpv[i / 3 + m][(i % 3) * 3 + n].possible_data & ( 1 << (v - 1))) {
                        c++;
                        dx = i / 3 + m;
                        dy = (i % 3) * 3 + n;
                    }
                }
           }
            if (c == 1) {
                    lm[dx][dy] = v; 
                    lpv[dx][dy].max = 1;
                    lpv[dx][dy].possible_data = 1 << (v - 1);
            }
        }
    }
    return 0;
}

int possible_value(int lm[][9], int i, int j, struct _p_v lpv[][9], unsigned long  pos_val)
{
   int k, m,n, d,  conflict = 0;
   memset(&lpv[i][j], 0, sizeof(struct _p_v));
   if (lm[i][j]) {
        lpv[i][j].max = 1;  
        lpv[i][j].possible_data = 1 << (lm[i][j] - 1);
        return 1;
   } 
  
   for (k = 1; k < 10; k++) {
      if (!(pos_val & (1 << (k - 1))))
          continue;

      conflict = 0; 
      /* line */  
      for (m = 0; m < 9; m++) {
        if (lm[i][m] == 0) {
            continue;
        }
        if (lm[i][m] == k) {
            conflict = 1;
            break;
        }
      } 
      if (conflict) {
        continue;
      }

      for (m = 0; m < 9; m++) {
        if (lm[m][j] == 0) {
            continue;
        }
        if (lm[m][j] == k) {
            conflict = 1;
            break;
        }
      } 
      if (conflict) {
        continue;
      }
    
      for (m = 0; m < 3; m++) {
        for (n = 0; n < 3; n++) {
           d = lm[i / 3 * 3 + m][j / 3 * 3 + n];
           if (d == 0) {
                continue;
           }
           if (d == k) {
                conflict = 1;
                break;
           }
        }
        if (conflict) {
            break;
        }
      } 
      if (conflict) {
        continue;
      }
      lpv[i][j].possible_data |= 1 << (k - 1);
      lpv[i][j].max++; 
   }
   if (lpv[i][j].max == 0){
        return -1;
   } else {
       printf("(%d, %d): %s\n",i, j, data_to_string(lpv[i][j].possible_data));
   }
   return lpv[i][j].max;
}

void find_min_pos(int *x, int *y, struct _p_v lpv[][9]) 
{
    int i, j, min = 10, max = 0;    
    if (x) *x = -1;
    if (y) *y = -1;
    for (i = 0; i < 9; i ++) {
            for (j = 0; j < 9; j++) {
               if (lpv[i][j].max != 1 &&  lpv[i][j].max < min) {
                   min = lpv[i][j].max; 
                   if (x) *x =  i;
                   if (y) *y =  j;
               }
               if (lpv[i][j].max != 1 &&  lpv[i][j].max > min) {
                   max = lpv[i][j].max; 
               }
            } 
    }
    gl_max = max;
}
int main(int argc, void *argv[])
{
    int ret, i, j, k; 
    
    FILE *fp; 
    char buf[256];    
    if (argv[1] != NULL) {
        fp = fopen((char *)argv[1], "r");
    } else {
        fp = fopen("sudo.txt", "r");
    }
    if (fp) {
        i = 0; j = 0;
        while(fgets(buf, sizeof(buf), fp)) {
                for (j = 0; j < 9; j++) {
                    matrix[i][j] = buf[j] - '0'; 
                }
                i ++; 
        }
        fclose(fp);
    } else {
        if (argv[1]) {
         printf("can't open the file %s\n", argv[1]);
        } else {
         printf("can't open the file %s\n", "sudo.txt");
        }
        return 0;
    }
    ret = solve_matrix(matrix, pv); 
    if (ret == 1) {
            find_min_pos(NULL, NULL, pv);
            if (gl_max >= 2) {
                ret = solve_matrix(matrix, pv); 
            }
    } else {
        printf("no result\n");
    }
}

