CS6.302 - Software System Development  
Lab 2 SQL - Stored Procedures and Cursors  

=============================================================
Instructions for Running Lab 2 Scripts
=============================================================

1. Database Setup
-----------------
- Open MySQL Workbench.
- Create a new schema (for example: campus).
- Run the given CREATE TABLE and INSERT statements from the assignment
  to create and populate the following tables:
    - Shows
    - Subscribers
    - WatchHistory

2. Running the Scripts
----------------------
- Execute the following SQL files in order:

   q1.sql → Creates procedure ListAllSubscribers()
   q2.sql → Creates procedure GetWatchHistoryBySubscriber(IN sub_id INT)
   q3.sql → Creates procedure AddSubscriberIfNotExists(IN subName VARCHAR(100))
   q4.sql → Creates procedure SendWatchTimeReport()
   q5.sql → Creates procedure PrintAllSubscribersWatchHistory()

3. Testing the Procedures
-------------------------
- Call each procedure as follows:

   CALL ListAllSubscribers();

   CALL GetWatchHistoryBySubscriber(1);
   -- Replace (1) with subscriber ID of your choice.

   CALL AddSubscriberIfNotExists('New User');
   -- Inserts a new subscriber if not already present.

   CALL SendWatchTimeReport();
   -- Prints watch history report for all subscribers who have watched something.

   CALL PrintAllSubscribersWatchHistory();
   -- Prints watch history for each subscriber.

4. Expected Outputs
-------------------
- q1: Prints all subscriber names (separate result sets).
- q2: Prints shows + watch time for a given subscriber.
- q3: Adds subscriber if not exists, otherwise prints a message.
- q4: Prints each subscriber’s watch history with header.
- q5: Iterates through subscribers and prints their watch history.

5. GitHub Repository
--------------------
GitHub Repo Link: https://github.com/suyashnpande/IIIT-Hyderabad/tree/main/Semester%201/SSD/2025201063_lab2


