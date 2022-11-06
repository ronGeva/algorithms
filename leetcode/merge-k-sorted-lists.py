from heapq import heappush, heappop


class ListElement:
    def __init__(self, node):
        self.node = node

    def __ge__(self, other):
        return self.node.val >= other.node.val


# Definition for singly-linked list.
class ListNode(object):
    def __init__(self, val=0, next=None):
        self.val = val
        self.next = next


class Solution(object):
    def mergeKLists(self, lists):
        """
        :type lists: List[ListNode]
        :rtype: ListNode
        """
        res = ListNode()
        start = res
        min_heap = []
        for i in range(len(lists)):
            if lists[i] is not None:
                heappush(min_heap, ListElement(lists[i]))
                lists[i] = lists[i].next

        while len(min_heap) > 0:
            res.next = ListNode()
            res = res.next
            smallest_item = heappop(min_heap)
            res.val = smallest_item.node.val
            if smallest_item.node.next is not None:
                heappush(min_heap, ListElement(smallest_item.node.next))
        return start.next