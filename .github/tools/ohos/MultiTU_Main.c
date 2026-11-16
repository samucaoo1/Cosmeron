int CosmeronOHOS_Detect(void);
int CosmeronOHOS_Status(void);
int main(void) { (void)CosmeronOHOS_Detect(); return CosmeronOHOS_Status() != 0; }
