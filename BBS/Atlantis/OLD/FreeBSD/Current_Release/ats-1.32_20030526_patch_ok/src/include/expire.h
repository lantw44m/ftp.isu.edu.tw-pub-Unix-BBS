#define DEF_DAYS        365            /* 定義 expire 文章 dated 時間 (日)  */
#define DEF_MAXP        3000           /* 定義 expire 文章 最大數量         */
#define DEF_MINP        2000           /* 定義 expire 文章砍除後剩於數量    */

#define Expire_ID  "SYSOP"    /* 定義可執行的 ID 反定義則判定 PERM_SYSOP    */
                              /* 請自行修改成需要的 User ID 或視需要 undef  */
#define Expire_Interval 3600 * 24 * 1  /* 定義執行每次 expire 區隔時間 (秒) */
#define Expire_Timeout  3600 * 2       /* 定義正常執行 expire 最大時間 (秒) */

