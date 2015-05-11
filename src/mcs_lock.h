
static inline void *xchg64(void *ptr, void *x)
{
        __asm__ __volatile__("xchgq %0,%1"
                                :"=r" ((unsigned long long) x)
                                :"m" (*(volatile long long *)ptr), "0" ((unsigned long long) x)
                                :"memory");

        return x;
}



class Mcs_Lock{
public:
	//typedef struct mcs_lock_struct mcs_lock_struct_t;
	struct mcs_lock_struct_t  {
		mcs_lock_struct_t *next ;	
		bool locked;
	};

	typedef mcs_lock_struct_t * mcs_lock;
        void init(){
		Lock=NULL;
        }
	void lock( mcs_lock_struct_t * local ){
		 mcs_lock_struct_t * pred;
		 local-> next = NULL;
		 pred= (mcs_lock_struct_t *) xchg64(&Lock,local);

		 if (pred){
			local->locked=true;
			pred->next=local;
			__asm__ __volatile__("": : :"memory");
			//int cntr=0;
                        while (local->locked){
					__asm__ __volatile__("pause\n": : :"memory");
			//		 cntr++;
					//if (cntr>100000) {
							//std::this_thread::yield();
					//		cntr=0;
				//	}
			}
		 }
	}

	void unlock(mcs_lock_struct_t * local ){
		if (local->next==NULL){
			if ( __sync_bool_compare_and_swap (&Lock,local, NULL)) return;
			//if ( __sync_val_compare_and_swap (&Lock,local, NULL)==local) return;
			while (local->next ==NULL){
				 __asm__ __volatile__("pause\n": : :"memory");
			}
		}
		local->next->locked=false;
	}


private:
	mcs_lock Lock;

};



