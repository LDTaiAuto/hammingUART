// Mã hóa hamming (12,8) : 8 bit data và 4 bit parity 
Cách mã hóa hamming: 
4 bit p1, p2, p4, p8 sẽ kiểm tra cac bit thuộc
        p1 = d[0] ^ d[1] ^ d[3] ^ d[4] ^ d[6]	; 
				p2 = d[0] ^ d[2] ^ d[3] ^ d[5] ^ d[6] ; 
				p4 = d[1] ^ d[2] ^ d[3] ^ d[7] ; 
				p8 = d[4] ^ d[5] ^ d[6] ^ d[7] ;
==> Sau đó chèn các bit p1, p2, p4, p8 đúng theo dạng Hamming (12,8)

   	uint16_t codeHam ; 
				codeHam |= (p1 << 0)	 		; 
				codeHam |= (p2 << 1) 			; 
				codeHam |= (d[0] << 2) 		; 
				codeHam |= (p4 << 3)  		; 
				codeHam |= (d[1] << 4)		;
				codeHam |= (d[2] << 5)  	;
				codeHam |= (d[3] << 6) 		; 
				codeHam |= (p8 << 7) 			;
				codeHam |= (d[4] << 8)		;
				codeHam |= (d[5] << 9)  	;
				codeHam |= (d[6] << 10)   ; 
				codeHam |= (d[7] << 11)   ; 

// đảo bit tạo lỗi 
chọn 1 bit bất kì để đảo bit ==> tạo lỗi hamming 
   codeHam ^= (1 << bitPos); 

Truyền và giải mã hamming 
// kiểm tra vị trí lỗi 
				s1 = bits[1] ^ bits[3] ^ bits[5] ^ bits[7] ^ bits[9] ^ bits[11] ; 
				s2 = bits[2] ^ bits[3] ^ bits[6] ^ bits[7] ^ bits[10] ^ bits[11]; 
				s4 = bits[4] ^ bits[5] ^ bits[6] ^ bits[7] ^ bits[12];
				s8 = bits[8] ^ bits[9] ^ bits[10] ^ bits[11] ^ bits[12]; 
				
===> Sửa vị trí lỗi :
    bits[errorPos] ^= 1 ; 
Trả về data ban đầu đã nhập.
    
// Nhập dữ liệu từ bàn phím 4x4 hoặc từ phần mêm Hecules Terminal 

Quy trình xử lý: 
Input data ==>  Error bit ==> Hamming Encode ==> Flipbit ==> Transmiter Hamming Fliped ==> Decode Hamming ==> Return Data input 
