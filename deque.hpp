#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <iostream>
#include <cstddef>

#define nodesize 200

namespace sjtu {

    template<class T>
    class deque {
        friend class iterator;
        friend class const_iterator;
        struct node
        {
            T* x;
            int tip;
            node* pre, * next;
            node(int ti, const T& _x, node* _pre = NULL, node* _nxt = NULL) { x = new T(_x); tip = ti; pre = _pre; next = _nxt; }
        };
        struct block
        {
            node* nhead;
            int Size;
            block* pre, * next;
            block(int _Size = 0, node* _nhead = NULL, block* _pre = NULL, block* _nxt = NULL) { nhead = _nhead; Size = _Size; pre = _pre; next = _nxt; }
        };
        block* blhead, * bltail;
        node* head, * tail;
        int len;

        int merge(block* bst)
        {
            block* bp = bst->next;
            if (bp == NULL) return 0;
            if (bst->Size + bp->Size <= nodesize)
            {
                bst->Size += bp->Size;
                bst->next = bp->next;
                bp->nhead->tip = 0;
                if (bp->next != NULL)
                    bp->next->pre = bst;
                if (bp == bltail) bltail = bst;
                delete bp;
                return 1;
            }
            return 0;
        }

        int split(block* bst)
        {
            if (bst->Size <= nodesize) return 0;
            node* p = bst->nhead;
            block* q;
            for (int i = 1; i <= bst->Size / 2; i++)
                p = p->next;
            p->tip = 1;
            q = bst->next;
            bst->next = new block(bst->Size - bst->Size / 2, p, bst, bst->next);
            if (q != NULL) q->pre = bst->next;
            if (bst == bltail) bltail = bst->next;
            bst->Size = bst->Size / 2;
            return 1;
        }
    public:
        class const_iterator;
        class iterator {
            friend class const_iterator;
            friend class deque;
        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
            deque* dp;
            block* bp;
            node* p;
        public:
            /**
             * return a new iterator which pointer n-next elements
             *   even if there are not enough elements, the behaviour is **undefined**.
             * as well as operator-
             */
            iterator(deque* dq, block* blk, node* nd) {
                dp = dq;
                bp = blk;
                p = nd;
            }
            iterator() {
                dp = NULL;
                bp = NULL;
                p = NULL;
            }
            iterator(const iterator& other) {
                dp = other.dp;
                bp = other.bp;
                p = other.p;
            }
            iterator operator+(const int& n) const {
                //TODO
                if (n < 0) return this->operator-(-n);
                int cou = n, on1 = 0;
                node* pi = p;
                if (cou == 0) return *this;
                do
                {
                    pi = pi->next;
                    cou--;
                } while (cou > 0 && pi != NULL && pi->tip == 0);
                if (cou == 0 || pi == NULL)
                {
                    if (pi != NULL && pi->tip == 1)
                        return iterator(dp, bp->next, pi);
                    else
                        return iterator(dp, bp, pi);
                }
                block* bpi = bp->next;
                //if (bpi == NULL) return iterator(dp, bpi, NULL);
                while (bpi != NULL && cou >= bpi->Size)
                {
                    cou -= bpi->Size;
                    bpi = bpi->next;
                }
                if (bpi == NULL) return dp->end();
                if (cou < bpi->Size / 2)
                {
                    pi = bpi->nhead;
                    while (cou > 0 && pi != NULL)
                    {
                        pi = pi->next;
                        cou--;
                    }
                    return iterator(dp, bpi, pi);
                }
                else
                {
                    if (bpi->next==NULL)
                    {
                        pi = dp->tail;
                        cou = bpi->Size - cou - 1;
                        int u = cou;
                        if (cou == -1) return dp->end();
                        while (cou--)
                        {
                            /*if (pi == NULL)
                            {
                                std::cout << u;
                            }*/
                            pi = pi->pre;
                        }
                        return iterator(dp, bpi, pi);
                    }
                    else
                    {
                        pi = bpi->next->nhead->pre;
                        cou = bpi->Size - cou - 1;
                        int u = cou;
                        while (cou--)
                            pi = pi->pre;
                        return iterator(dp, bpi, pi);
                    }
                }
            }
            iterator operator-(const int& n) const {
                //TODO
                if (n < 0) return this->operator+(-n);
                int cou = n;
                if (cou == 0) return *this;
                iterator pos = *this;
                if (*this == dp->end())
                {
                    cou--;
                    pos.bp = dp->bltail;
                    pos.p = dp->tail;
                }
                node* pi = pos.p;
                while (cou > 0 && pi->tip == 0)
                {
                    cou--;
                    pi = pi->pre;
                }
                if (cou == 0) return iterator(pos.dp, pos.bp, pi);
                block* bpi = pos.bp->pre;
                while (bpi != NULL && cou >= bpi->Size)
                {
                    cou -= bpi->Size;
                    bpi = bpi->pre;
                }
                if (bpi == NULL) return dp->begin();
                pi = bpi->next->nhead;
                if (cou == 0) return iterator(pos.dp, bpi->next, pi);
                if (cou < bpi->Size / 2)
                {
                    while (cou > 0 && pi != NULL)
                    {
                        pi = pi->pre;
                        cou--;
                    }
                    return iterator(pos.dp, bpi, pi);
                }
                else
                {
                    cou = bpi->Size - cou;
                    pi = bpi->nhead;
                    while (cou--)
                        pi = pi->next;
                    return iterator(pos.dp, bpi, pi);
                }
            }
            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
            int operator-(const iterator& rhs) const {
                //TOD
                if (dp != rhs.dp) throw invalid_iterator();
                int re = 0;
                if (bp == rhs.bp)
                {
                    node* pi = bp->nhead;
                    while (pi != p && pi != rhs.p)
                        pi = pi->next;
                    if (pi == p)
                    {
                        while (pi != rhs.p)
                        {
                            pi = pi->next;
                            re--;
                        }
                    }
                    else
                    {
                        while (pi != p)
                        {
                            pi = pi->next;
                            re++;
                        }
                    }
                    return re;
                }
                else
                {
                    block* bpi = dp->blhead;
                    while (bpi != bp && bpi != rhs.bp)
                        bpi = bpi->next;
                    if (bpi == bp)
                    {
                        node* pi = p;
                        do
                        {
                            re--;
                            pi = pi->next;
                        } while (pi->tip == 0);
                        bpi = bpi->next;
                        while (bpi != rhs.bp)
                        {
                            re -= bpi->Size;
                            bpi = bpi->next;
                        }
                        pi = bpi->nhead;
                        while (pi != rhs.p)
                        {
                            pi = pi->next;
                            re--;
                        }
                        return re;
                    }
                    else
                    {
                        node* pi = rhs.p;
                        do
                        {
                            re++;
                            pi = pi->next;
                        } while (pi->tip == 0);
                        bpi = bpi->next;
                        while (bpi != bp)
                        {
                            re += bpi->Size;
                            bpi = bpi->next;
                        }
                        pi = bpi->nhead;
                        while (pi != p)
                        {
                            pi = pi->next;
                            re++;
                        }
                        return re;
                    }
                }
            }
            iterator& operator+=(const int& n) {
                //TODO
                *this = (*this) + n;
                return *this;
            }
            iterator& operator-=(const int& n) {
                //TODO
                *this = (*this) - n;
                return *this;
            }
            /**
             * TODO iter++
             */
            iterator operator++(int)
            {
                if (p == NULL) throw invalid_iterator();
                iterator re = *this;
                p = p->next;
                if (p != NULL)
                {
                    if (p->tip == 1)
                    {
                        bp = bp->next;
                        if (bp == NULL)
                            throw invalid_iterator();
                    }
                }
                if (p == NULL) { *this = dp->end(); return re; }
                return re;
            }
            /**
             * TODO ++iter
             */
            iterator& operator++()
            {
                if (p == NULL) throw invalid_iterator();
                p = p->next;
                if (p != NULL)
                {
                    if (p->tip == 1)
                    {
                        bp = bp->next;
                        if (bp == NULL)
                            throw invalid_iterator();
                    }
                }
                if (p == NULL) { *this = dp->end(); return *this; }
                return *this;
            }
            /**
             * TODO iter--
             */
            iterator operator--(int)
            {
                iterator re = *this;
                if (*this == dp->end())
                {
                    this->bp = dp->bltail;
                    this->p = dp->tail;
                    return re;
                }
                if (p->tip == 1)
                {
                    bp = bp->pre;
                    if (bp == NULL)
                        throw invalid_iterator();
                }
                p = p->pre;
                return re;
            }
            /**
             * TODO --iter
             */
            iterator& operator--()
            {
                if (*this == dp->end())
                {
                    this->bp = dp->bltail;
                    this->p = dp->tail;
                    return *this;
                }
                if (p->tip == 1)
                {
                    bp = bp->pre;
                    if (bp == NULL)
                        throw invalid_iterator();
                }
                p = p->pre;
                return *this;
            }
            /**
             * TODO *it
             */
            T& operator*() const
            {
                if (dp == NULL) throw invalid_iterator();
                if (bp == NULL) throw invalid_iterator();
                if (p == NULL) throw invalid_iterator();
                return *(p->x);
            }
            /**
             * TODO it->field
             */
            T* operator->() const noexcept
            {
                if (dp == NULL) throw invalid_iterator();
                if (bp == NULL) throw invalid_iterator();
                if (p == NULL) throw invalid_iterator();
                return p->x;
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator& rhs) const { return dp == rhs.dp && bp == rhs.bp && p == rhs.p; }
            bool operator==(const const_iterator& rhs) const { return dp == rhs.dp && bp == rhs.bp && p == rhs.p; }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator& rhs) const { return !(dp == rhs.dp && bp == rhs.bp && p == rhs.p); }
            bool operator!=(const const_iterator& rhs) const { return !(dp == rhs.dp && bp == rhs.bp && p == rhs.p); }
        };
        class const_iterator {
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
            friend class iterator;
            friend class deque;
        private:
            // data members.
            const deque* dp;
            const block* bp;
            const node* p;
        public:
            const_iterator() {
                // TODO
                p = NULL; dp = NULL; bp = NULL;
            }
            const_iterator(const deque* dq, const block* blk, const node* nd) :dp(dq), bp(blk), p(nd) {
            }
            const_iterator(const const_iterator& other) {
                // TODO
                p = other.p; dp = other.dp; bp = other.bp;
            }
            const_iterator(const iterator& other) {
                // TODO
                p = other.p; dp = other.dp; bp = other.bp;
            }
            // And other methods in iterator.
            // And other methods in iterator.
            // And other methods in iterator.
            const_iterator operator+(const int& n) const {
                //TODO
                if (n < 0) return this->operator-(-n);
                int cou = n, on1 = 0;
                const node* pi = p;
                if (cou == 0) return *this;
                do
                {
                    pi = pi->next;
                    cou--;
                } while (cou > 0 && pi != NULL && pi->tip == 0);
                if (cou == 0 || pi == NULL)
                {
                    if (pi != NULL && pi->tip == 1)
                        return const_iterator(dp, bp->next, pi);
                    else
                        return const_iterator(dp, bp, pi);
                }
                block* bpi = bp->next;
                //if (bpi == NULL) return iterator(dp, bpi, NULL);
                while (bpi != NULL && cou >= bpi->Size)
                {
                    cou -= bpi->Size;
                    bpi = bpi->next;
                }
                if (bpi == NULL) return dp->cend();
                pi = bpi->nhead;
                while (cou > 0 && pi != NULL)
                {
                    pi = pi->next;
                    cou--;
                }
                return const_iterator(dp, bpi, pi);
            }
            const_iterator operator-(const int& n) const {
                //TODO
                if (n < 0) return this->operator+(-n);
                int cou = n;
                if (cou == 0) return *this;
                const_iterator pos = *this;
                if (*this == dp->cend())
                {
                    cou--;
                    pos.bp = dp->bltail;
                    pos.p = dp->tail;
                }
                const node* pi = pos.p;
                while (cou > 0 && pi->tip == 0)
                {
                    cou--;
                    pi = pi->pre;
                }
                if (cou == 0) return const_iterator(pos.dp, pos.bp, pi);
                const block* bpi = pos.bp->pre;
                while (bpi != NULL && cou >= bpi->Size)
                {
                    cou -= bpi->Size;
                    bpi = bpi->pre;
                }
                if (bpi == NULL) return dp->cbegin();
                pi = bpi->next->nhead;
                if (cou == 0) return const_iterator(pos.dp, bpi->next, pi);
                while (cou > 0 && pi != NULL)
                {
                    pi = pi->pre;
                    cou--;
                }
                return const_iterator(pos.dp, bpi, pi);
            }
            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
            int operator-(const const_iterator& rhs) const {
                //TOD
                if (dp != rhs.dp) throw invalid_iterator();
                int re = 0;
                if (bp == rhs.bp)
                {
                    node* pi = bp->nhead;
                    while (pi != p && pi != rhs.p)
                        pi = pi->next;
                    if (pi == p)
                    {
                        while (pi != rhs.p)
                        {
                            pi = pi->next;
                            re--;
                        }
                    }
                    else
                    {
                        while (pi != p)
                        {
                            pi = pi->next;
                            re++;
                        }
                    }
                    return re;
                }
                else
                {
                    const block* bpi = dp->blhead;
                    while (bpi != bp && bpi != rhs.bp)
                        bpi = bpi->next;
                    if (bpi == bp)
                    {
                        const node* pi = p;
                        do
                        {
                            re--;
                            pi = pi->next;
                        } while (pi->tip == 0);
                        bpi = bpi->next;
                        while (bpi != rhs.bp)
                        {
                            re -= bpi->Size;
                            bpi = bpi->next;
                        }
                        pi = bpi->nhead;
                        while (pi != rhs.p)
                        {
                            pi = pi->next;
                            re--;
                        }
                        return re;
                    }
                    else
                    {
                        const node* pi = rhs.p;
                        do
                        {
                            re++;
                            pi = pi->next;
                        } while (pi->tip == 0);
                        bpi = bpi->next;
                        while (bpi != bp)
                        {
                            re += bpi->Size;
                            bpi = bpi->next;
                        }
                        pi = bpi->nhead;
                        while (pi != p)
                        {
                            pi = pi->next;
                            re++;
                        }
                        return re;
                    }
                }
            }
            const_iterator& operator+=(const int& n) {
                //TODO
                *this = (*this) + n;
                return *this;
            }
            const_iterator& operator-=(const int& n) {
                //TODO
                *this = (*this) - n;
                return *this;
            }
            /**
             * TODO iter++
             */
            const_iterator operator++(int)
            {
                if (p == NULL) throw invalid_iterator();
                const_iterator re = *this;
                p = p->next;
                if (p != NULL)
                {
                    if (p->tip == 1)
                    {
                        bp = bp->next;
                        if (bp == NULL)
                            throw invalid_iterator();
                    }
                }
                if (p == NULL) { *this = dp->end(); return re; }
                return re;
            }
            /**
             * TODO ++iter
             */
            const_iterator& operator++()
            {
                if (p == NULL) throw invalid_iterator();
                p = p->next;
                if (p != NULL)
                {
                    if (p->tip == 1)
                    {
                        bp = bp->next;
                        if (bp == NULL)
                            throw invalid_iterator();
                    }
                }
                if (p == NULL) { *this = dp->cend(); return *this; }
                return *this;
            }
            /**
             * TODO iter--
             */
            const_iterator operator--(int)
            {
                const_iterator re = *this;
                if (*this == dp->cend())
                {
                    this->bp = dp->bltail;
                    this->p = dp->tail;
                    return re;
                }
                if (p->tip == 1)
                {
                    bp = bp->pre;
                    if (bp == NULL)
                        throw invalid_iterator();
                }
                p = p->pre;
                return re;
            }
            /**
             * TODO --iter
             */
            const_iterator& operator--()
            {
                if (*this == dp->cend())
                {
                    this->bp = dp->bltail;
                    this->p = dp->tail;
                    return *this;
                }
                if (p->tip == 1)
                {
                    bp = bp->pre;
                    if (bp == NULL)
                        throw invalid_iterator();
                }
                p = p->pre;
                return *this;
            }
            /**
             * TODO *it
             */
            T& operator*() const
            {
                if (dp == NULL) throw invalid_iterator();
                if (bp == NULL) throw invalid_iterator();
                if (p == NULL) throw invalid_iterator();
                return *(p->x);
            }
            /**
             * TODO it->field
             */
            T* operator->() const noexcept
            {
                if (dp == NULL) throw invalid_iterator();
                if (bp == NULL) throw invalid_iterator();
                if (p == NULL) throw invalid_iterator();
                return p->x;
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator& rhs) const { return dp == rhs.dp && bp == rhs.bp && p == rhs.p; }
            bool operator==(const const_iterator& rhs) const { return dp == rhs.dp && bp == rhs.bp && p == rhs.p; }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator& rhs) const { return !(dp == rhs.dp && bp == rhs.bp && p == rhs.p); }
            bool operator!=(const const_iterator& rhs) const { return !(dp == rhs.dp && bp == rhs.bp && p == rhs.p); }
        };
        /**
         * TODO Constructors
         */
        deque() { blhead = NULL; bltail = NULL; head = NULL; tail = NULL; len = 0; }
        deque(const deque& other)
        {
            node* p = other.head;
            if (p == NULL)
            {
                blhead = NULL; bltail = NULL; head = NULL; tail = NULL; len = 0;
                return;
            }
            len = other.len;
            head = tail = new node(1, *(p->x));
            blhead = bltail = new block(1, tail);
            for (size_t i = 1; i < len; i++)
            {
                p = p->next;
                tail = tail->next = new node(0, *(p->x), tail);
                if (bltail->Size == nodesize)
                {
                    bltail = bltail->next = new block(1, tail, bltail);
                    tail->tip = 1;
                }
                else
                    bltail->Size++;
            }
        }
        /**
         * TODO Deconstructor
         */
        ~deque() { clear(); }
        /**
         * TODO assignment operator
         */
        deque& operator=(const deque& other)
        {
            if (this == &other) return *this;
            node* p = other.head;
            clear();
            if (p == NULL)
            {
                blhead = NULL; bltail = NULL; head = NULL; tail = NULL; len = 0;
                return *this;
            }
            len = other.len;
            head = tail = new node(1, *(p->x));
            blhead = bltail = new block(1, tail);
            for (size_t i = 1; i < len; i++)
            {
                p = p->next;
                tail = tail->next = new node(0, *(p->x), tail);
                if (bltail->Size == nodesize)
                {
                    bltail = bltail->next = new block(1, tail, bltail);
                    tail->tip = 1;
                }
                else
                    bltail->Size++;
            }
            return *this;
        }
        /**
         * access specified element with bounds checking
         * throw index_out_of_bound if out of bound.
         */
        T& at(const size_t& pos)
        {
            if (pos >= len) throw index_out_of_bound();
            if (pos < len / 2)
            {
                block* bp = blhead;
                size_t cou = pos;
                while (cou >= bp->Size)
                {
                    cou -= bp->Size;
                    bp = bp->next;
                }
                node* p = bp->nhead;
                while (cou--)
                    p = p->next;
                return *(p->x);
            }
            else
            {
                iterator re(this, bltail, tail);
                int cou = len - 1 - pos;
                re -= cou;
                return *re;
            }
        }
        const T& at(const size_t& pos) const {
            if (pos >= len) throw index_out_of_bound();
            if (pos < len / 2)
            {
                block* bp = blhead;
                size_t cou = pos;
                while (cou >= bp->Size)
                {
                    cou -= bp->Size;
                    bp = bp->next;
                }
                node* p = bp->nhead;
                while (cou--)
                    p = p->next;
                return *(p->x);
            }
            else
            {
                const_iterator re(this, bltail, tail);
                int cou = len - 1 - pos;
                re -= cou;
                return *re;
            }
        }
        T& operator[](const size_t& pos) {
            if (pos >= len) throw index_out_of_bound();
            if (pos < len / 2)
            {
                block* bp = blhead;
                size_t cou = pos;
                while (cou >= bp->Size)
                {
                    cou -= bp->Size;
                    bp = bp->next;
                }
                node* p = bp->nhead;
                while (cou--)
                    p = p->next;
                return *(p->x);
            }
            else
            {
                iterator re(this, bltail, tail);
                int cou = len - 1 - pos;
                re -= cou;
                return *re;
            }
        }
        const T& operator[](const size_t& pos) const {
            if (pos >= len) throw index_out_of_bound();
            if (pos < len / 2)
            {
                block* bp = blhead;
                size_t cou = pos;
                while (cou >= bp->Size)
                {
                    cou -= bp->Size;
                    bp = bp->next;
                }
                node* p = bp->nhead;
                while (cou--)
                    p = p->next;
                return *(p->x);
            }
            else
            {
                const_iterator re(this, bltail, tail);
                int cou = len - 1 - pos;
                re -= cou;
                return *re;
            }
        }
        /**
         * access the first element
         * throw container_is_empty when the container is empty.
         */
        const T& front() const { if (len == 0) throw container_is_empty(); return *(head->x); }
        /**
         * access the last element
         * throw container_is_empty when the container is empty.
         */
        const T& back() const { if (len == 0) throw container_is_empty(); return *(tail->x); }
        /**
         * returns an iterator to the beginning.
         */
        iterator begin() { return iterator(this, blhead, head); }
        const_iterator cbegin() const { return const_iterator(this, blhead, head); }
        /**
         * returns an iterator to the end.
         */
        iterator end() { return iterator(this, bltail, NULL); }
        const_iterator cend() const { return const_iterator(this, bltail, NULL); }
        /**
         * checks whether the container is empty.
         */
        bool empty() const { return len == 0; }
        /**
         * returns the number of elements
         */
        size_t size() const { return len; }
        /**
         * clears the contents
         */
        void clear()
        {
            node* p = head, * q;
            while (p != NULL)
            {
                q = p->next;
                delete p->x;
                delete p;
                p = q;
            }
            block* bp = blhead, * bq;
            while (bp != NULL)
            {
                bq = bp->next;
                delete bp;
                bp = bq;
            }
            blhead = NULL; bltail = NULL; head = NULL; tail = NULL; len = 0;
        }
        /**
         * inserts elements at the specified locat on in the container.
         * inserts value before pos
         * returns an iterator pointing to the inserted value
         *     throw if the iterator is invalid or it point to a wrong place.
         */
        iterator insert(iterator pos, const T& value)
        {
            if (pos.dp != this) throw invalid_iterator();
            if (pos == end()) { push_back(value); return iterator(this, bltail, tail); }
            if (pos == begin()) { push_front(value); return iterator(this, blhead, head); }
            if (pos.p == NULL || pos.bp == NULL) throw invalid_iterator();
            node* p = pos.p, * q = p->pre;
            len++;
            p->pre = new node(0, value, p->pre, p);
            if (p->tip == 1)
            {
                pos.bp->nhead = p->pre;
                p->pre->tip = 1;
                p->tip = 0;
            }
            if (q != NULL)
                q->next = p->pre;
            pos.bp->Size++;
            if (split(pos.bp))
            {
                int u = 0;
                node* tp = pos.bp->nhead;
                for (int i = 0; i < pos.bp->Size; i++, tp = tp->next)
                    if (tp == p->pre)
                    {
                        u = 1;
                        break;
                    }
                if (u == 1)
                    return iterator(this, pos.bp, p->pre);
                else
                    return iterator(this, pos.bp->next, p->pre);
            }
            else
                return iterator(this, pos.bp, p->pre);
        }
        /**
         * removes specified element at pos.
         * removes the element at pos.
         * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
         * throw if the container is empty, the iterator is invalid or it points to a wrong place.
         */
        iterator erase(iterator pos)
        {
            if (pos.dp != this) throw invalid_iterator();
            if (len == 0) throw container_is_empty();
            if (pos == begin()) { pop_front(); return begin(); }
            if (pos == iterator(this, bltail, tail)) { pop_back(); return end(); }
            if (pos.p == NULL || pos.bp == NULL) throw invalid_iterator();
            node* p = pos.p, * q = p->pre;
            len--;
            block* bp = pos.bp;
            if (q != NULL) q->next = p->next;
            if (p->next != NULL) p->next->pre = q;
            if (bp->Size == 1)
            {
                q = p->next;
                delete p->x;
                delete p;
                if (pos.bp->pre != NULL) pos.bp->pre->next = pos.bp->next;
                if (pos.bp->next != NULL) pos.bp->next->pre = pos.bp->pre;
                bp = pos.bp->next;
                delete pos.bp;
                merge(bp);
                return iterator(this, bp, q);
            }
            else
            {
                q = p->next;
                int u = q->tip;
                if (p->tip == 1)
                {
                    p->next->tip = 1;
                    bp->nhead = p->next;
                }
                bp->Size--;
                delete p->x;
                delete p;
                if (merge(bp))
                    return iterator(this, bp, q);
                else if (q != NULL && u == 1)
                    return iterator(this, bp->next, q);
                else
                    return iterator(this, bp, q);
            }
        }
        /**
         * adds an element to the end
         */
        void push_back(const T& value)
        {
            len++;
            if (tail == NULL)
            {
                head = tail = new node(1, value);
                blhead = bltail = new block(1, tail);
                return;
            }
            tail = tail->next = new node(0, value, tail);
            bltail->Size++;
            split(bltail);
        }
        /**
         * removes the last element
         *     throw when the container is empty.
         */
        void pop_back()
        {
            if (len == 0) throw container_is_empty();
            node* p = tail;
            block* bp = bltail;
            len--;
            tail = tail->pre;
            delete p->x;
            delete p;
            if (tail != NULL)
                tail->next = NULL;
            if (tail == NULL) head = NULL;
            if (bltail->Size == 1)
            {
                bltail = bltail->pre;
                if (bltail != NULL)
                    bltail->next = NULL;
                delete bp;
                if (bltail == NULL) blhead = NULL;
            }
            else
                bltail->Size--;
        }
        /**
         * inserts an element to the beginning.
         */
        void push_front(const T& value)
        {
            len++;
            if (head == NULL)
            {
                head = tail = new node(1, value);
                blhead = bltail = new block(1, tail);
                return;
            }
            head->pre = new node(1, value, NULL, head);
            head->tip = 0;
            head = head->pre;
            blhead->Size++;
            blhead->nhead = head;
            split(blhead);
        }
        /**
         * removes the first element.
         *     throw when the container is empty.
         */
        void pop_front()
        {
            if (len == 0) throw container_is_empty();
            node* p = head;
            block* bp = blhead;
            len--;
            head = head->next;
            delete p->x;
            delete p;
            if (head != NULL)
                head->pre = NULL;
            if (head == NULL) tail = NULL;
            if (blhead->Size == 1)
            {
                blhead = blhead->next;
                if (blhead != NULL)
                    blhead->pre = NULL;
                delete bp;
                if (blhead == NULL) bltail = NULL;
            }
            else
            {
                blhead->Size--;
                blhead->nhead = head;
                head->tip = 1;
            }
        }
    };

}

#endif
