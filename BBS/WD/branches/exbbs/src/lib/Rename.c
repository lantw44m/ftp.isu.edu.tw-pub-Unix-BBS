Rename(char* src, char* dst)
{
  if (rename(src, dst) == 0)
    return 0;    /* Ptt debuged */
  return -1;
}
