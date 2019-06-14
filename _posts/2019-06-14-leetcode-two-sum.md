---
title: LeetCode#1 Two Sum
layout: post
tags:
- LeetCode
- Algorithm
published: true
comments: true
---

## 題目
Given an array of integers, return  **indices**  of the two numbers such that they add up to a specific target.

You may assume that each input would have  **_exactly_**  one solution, and you may not use the  _same_  element twice.

**Example:**
```
Given nums = [2, 7, 11, 15], target = 9,

Because nums[0] + nums[1] = 2 + 7 = 9,
return [0, 1].
```

### 解法1 - 暴力法

非常直觀，就針對每個element往後掃，要注意的是因為input是 **signed integer** 所以不能因為``nums[i] > target`` 或 ``nums[i] < target`` 就跳掉inner loop

```cpp
vector<int> twoSum(vector<int>& nums, int target) {
    vector<int> res;
    for(int i = 0; i < nums.size() - 1; i++){
        // find from next element until end
        for(int j = i + 1; j < nums.size(); j++){
            if(nums[i] + nums[j] == target){
                // solution found
                res.push_back(i);
                res.push_back(j);
                break;
            }
        }
    }
    return res;
}
```
Time Complexity : $${O(N^2)}$$

Space Complexity : $${O(1)}$$

### 解法2 - 排序法

先將array由小到大排序過之後就可以由左右兩邊夾，但是要注意：因為這題要求的 output 是 index，不是 element 的值，所以**排序過後 index 就不是原本順序了**，要用額外的array記index，浪費空間且Code比較冗長，所以不是非常好的解法。<br>
另外，也不能用一個hash來記index，因為element可能會重複，就被蓋掉了，比方說[3, 3] 找6，答案會變成[1,1]。
 
```cpp
// customized sort for element value
static bool mysort(pair<int,int> a,pair<int,int> b){
    return a.first < b.first;
}

vector<int> twoSum(vector<int>& nums, int target) {
    // Because the return value is index of nums rather than value, the nums will be out of order after sort.
    // Thus, we need to store the original index before sort.
    vector<pair<int, int> > tmp;
    for(int i = 0 ; i < nums.size(); i++){
        // pair : {element, index}
        pair<int, int> x = pair<int, int>(nums[i],i);
        tmp.push_back(x);
    }
    
    std::sort(tmp.begin(), tmp.end(), mysort);
    
    int left = 0, right = tmp.size() - 1;
    while(1){
        if(tmp[left].first + tmp[right].first > target){
            // bigger than target, find smaller one
            right--;
        }
        else if(tmp[left].first + tmp[right].first < target){
            // smaller than target, find bigger one
            left++;
        }
        else {
            // answer found
            break;   
        }
    }
    
    // return the answer from small to big,
    // if the order is not limited, we can remove the if condition and keep arbitrary one
    vector<int> ret;
    
    if(tmp[left].second > tmp[right].second){
        ret.push_back(tmp[right].second);
        ret.push_back(tmp[left].second);
    }
    else{
        ret.push_back(tmp[left].second);
        ret.push_back(tmp[right].second);
    }
    
    return ret;
    
}
```

Time Complexity : 找答案是 $${O(N)}$$，但是要先排序過所以是 $${O(N\log{}N)}$$

Space Complexity :  `vector<pair<int, int> > tmp` : $${O(N)}$$

### 解法3 (最佳解) - Hash

一開始看到這個解法覺得很玄，仔細想了一下覺得也沒那麼難！概念是用一個Hash來存入每個元素的補數  `complement = target - element` 與其index。

假設 `nums = {a,b,c}, target = a+c` ，跑一個迴圈計算 complement 並將之存入hash，當作 hash的key，用index為hash的值。

* hash[target - a] = 0
* hash[target - b] = 1
* hash[target - c] = 2 (這個例子在算到C即找到答案，所以實際上不會有這個值)

當目前的元素(c) 能在 Hash的 key = target - a 裡面找到時，代表我們找到了 **c為a的補數 (a + c = target)**，也就是 `nums[hash[nums[i]] + nums[i] = target`，所以 `(hash[nums[i]], i)` 即為答案。


```cpp
vector<int> twoSum(vector<int>& nums, int target) {
  map<int, int> tmp;  // key: complement, value: index
  vector<int> ret;
  for(int i = 0 ; i < nums.size();i++){
      if(tmp.find(nums[i]) != tmp.end()){ // answer found
          ret.push_back(tmp[nums[i]]);    // index of previous element which complement equal to nums[i]
          ret.push_back(i);               // index of current element
          break;
      }
      int complement = target - nums[i];
      tmp[complement] = i;    // store index of current element, key is used for finding the complement
  }
  
  return ret;
}
```


Time Complexity : Hash的insert與find皆為 $${O(1)}$$ ([Reference](http://javabypatel.blogspot.com/2015/10/time-complexity-of-hashmap-get-and-put-operation.html))，Traverse nums : $${O(N)}$$

Space Complexity : `map<int, int> tmp` : $${O(N)}$$