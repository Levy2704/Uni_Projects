using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Windows.Forms;
using System.Data.SQLite;

//https://github.com/AbleOpus/NetworkingSamples/blob/master/MultiServer/Program.cs
namespace Windows_Forms_Chat
{
    // Database SQL Stuff
    class SqlDataBase
    {
        static void DataBaseSQL(string[] args)
        {
            string connectionString = "Data Source= TicTactoeDB.db";
            SQLiteConnection connection = new SQLiteConnection(connectionString);
            connection.Open(); // if testDBnot exist, create, Else, openit

            //check version of SQLite is running
            string statement = "SELECT SQLITE_VERSION()";
            SQLiteCommand cmd = new SQLiteCommand(statement, connection);
            
            //executeScaler gives 1 value back(1st row, 1st column if lots of data from select)
            string version = cmd.ExecuteScalar().ToString();

            Console.WriteLine("Sqlite version: " + version);
            //LETS BUILD A TABLE
            cmd = new SQLiteCommand("CREATE TABLE IF NOT EXISTS TicTacToe(TicTacToeID INTEGER PRIMARY KEY, username, password, wins, losses, draws)", connection);
            cmd.ExecuteNonQuery();
            connection.Close(); // save changes permanetly to db
        }
    }

    public class TCPChatServer : TCPChatBase
    {
        string connectionString = "Data Source= TicTactoeDB.db";
        SQLiteConnection connection;
        public TicTacToe ticTacToe;

        public Socket serverSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        //connected clients
        public List<ClientSocket> clientSockets = new List<ClientSocket>();

        public static TCPChatServer createInstance(int port, TextBox chatTextBox)
        { 
            
            TCPChatServer tcp = null;
            //setup if port within range and valid chat box given
            if (port > 0 && port < 65535 && chatTextBox != null)
            {
                tcp = new TCPChatServer();
                tcp.port = port;
                tcp.chatTextBox = chatTextBox;

            }

            //return empty if user not enter useful details
            return tcp;
        }

        public void SetupServer()
        {
            connection = new SQLiteConnection(connectionString);
            connection.Open(); // if testDBnot exist, create, Else, openit

            //check version of SQLite is running
            string statement = "SELECT SQLITE_VERSION()";
            SQLiteCommand cmd = new SQLiteCommand(statement, connection);

            //executeScaler gives 1 value back(1st row, 1st column if lots of data from select)
            string version = cmd.ExecuteScalar().ToString();

            Console.WriteLine("Sqlite version: " + version);
            //LETS BUILD A TABLE
            cmd = new SQLiteCommand("CREATE TABLE IF NOT EXISTS TicTacToe(TicTacToeID INTEGER PRIMARY KEY AUTOINCREMENT, username, password, wins, losses, draws)", connection);
            cmd.ExecuteNonQuery();

            chatTextBox.Text += "Setting up server...\n";
            serverSocket.Bind(new IPEndPoint(IPAddress.Any, port));
            serverSocket.Listen(0);
            //kick off thread to read connecting clients, when one connects, it'll call out AcceptCallback function
            serverSocket.BeginAccept(AcceptCallback, this);
            chatTextBox.Text += "Server setup complete\n";
        }



        public void CloseAllSockets()
        {
            foreach (ClientSocket clientSocket in clientSockets)
            {
                clientSocket.socket.Shutdown(SocketShutdown.Both);
                clientSocket.socket.Close();
            }
            clientSockets.Clear();
            serverSocket.Close();
        }


        public void AcceptCallback(IAsyncResult AR)
        {
            Socket joiningSocket;

            try
            {
                joiningSocket = serverSocket.EndAccept(AR);
            }
            catch (ObjectDisposedException) // I cannot seem to avoid this (on exit when properly closing sockets)
            {
                return;
            }

            ClientSocket newClientSocket = new ClientSocket();
            newClientSocket.socket = joiningSocket;

            clientSockets.Add(newClientSocket);
            //start a thread to listen out for this new joining socket. Therefore there is a thread open for each client
            joiningSocket.BeginReceive(newClientSocket.buffer, 0, ClientSocket.BUFFER_SIZE, SocketFlags.None, ReceiveCallback, newClientSocket);
            AddToChat("Client connected, waiting for request...");

            //we finished this accept thread, better kick off another so more people can join
            serverSocket.BeginAccept(AcceptCallback, null);
        }

        public void ReceiveCallback(IAsyncResult AR)
        {
            ClientSocket currentClientSocket = (ClientSocket)AR.AsyncState;

            int received;

            try
            {
                received = currentClientSocket.socket.EndReceive(AR);
            }
            catch (SocketException)
            {
                AddToChat("Client forcefully disconnected");
                // Don't shutdown because the socket may be disposed and its disconnected anyway.
                currentClientSocket.socket.Close();
                clientSockets.Remove(currentClientSocket);
                return;
            }

            byte[] recBuf = new byte[received];
            Array.Copy(currentClientSocket.buffer, recBuf, received);
            string text = Encoding.ASCII.GetString(recBuf);
            string[] sub = text.Split(' ');
            bool nameUsed = false;
            AddToChat(text);
            // -----------Comanands-----------------------------------
            // Add Username and password login sucessfully or not 
            if (sub[0] == "!login")
            {
                // foreach(ClientSocket login in clientSockets)

                SQLiteCommand cmd = new SQLiteCommand("", connection);
                cmd.CommandText = "SELECT * FROM TicTacToe WHERE username = '" + sub[1] + "'";
                SQLiteDataReader rdr = cmd.ExecuteReader();

                // If this is true found username
                if (rdr.Read())
                {
                    //Login succesful cout Welcome back user ###
                    //String PASS = rdr.GetString(2);
                    if (sub[2] == rdr.GetString(2))
                    {
                        currentClientSocket.username = sub[1];
                        byte[] dataL = Encoding.ASCII.GetBytes("Welcome back  " + sub[1] + " you're currently chatting");
                        currentClientSocket.socket.Send(dataL);
                        AddToChat(" User " + sub[1] + " enter the chat ");
                        currentClientSocket.state = clientState.chatting;

                    }
                    else // Client messed up 
                    {
                        byte[] data_I = Encoding.ASCII.GetBytes(sub[1] + " Username or password incorrect please try again");
                        currentClientSocket.socket.Send(data_I);
                    }
                }
                else //Client added succesfully
                {
                    SQLiteCommand cmd2 = new SQLiteCommand();
                    cmd2.Connection = connection;
                    cmd2.CommandText = "INSERT INTO TicTacToe(username, password, wins, losses, draws) VALUES( '" + sub[1] + "', '" + sub[2] + "' ,0, 0, 0)";
                    int rowsAffected = cmd2.ExecuteNonQuery();
                    currentClientSocket.username = sub[1];
                    byte[] dataS = Encoding.ASCII.GetBytes(" Username and password succefully added! You're currently in chatting state");
                    currentClientSocket.state = clientState.chatting;
                    currentClientSocket.socket.Send(dataS);
                    AddToChat("User " + sub[1] + " enter the chat");
                }


            }
            if (sub[0] == "!username")
            {
                //Loop to check al the usernames in list
                foreach (ClientSocket userName in clientSockets)
                {

                    if (sub[1] == userName.username)
                    { //if usernames is the same as sub 1 return message
                        byte[] data_U = Encoding.ASCII.GetBytes(sub[1] + "  Name already in use ");
                        currentClientSocket.socket.Send(data_U);
                        nameUsed = true;
                        break;
                    }
                    else if (nameUsed == false)
                    {// if username has not being used take that name
                        currentClientSocket.username = sub[1];
                        byte[] data = Encoding.ASCII.GetBytes(" It works, your username is " + sub[1] + " you're currently in chatting state");
                        currentClientSocket.socket.Send(data);
                        AddToChat(" User " + currentClientSocket.username + " enter the chat ");

                    }
                }
            }
            // change name if you already have one same principles as above
            if (sub[0] == "!user")
            {
                foreach (ClientSocket user in clientSockets)
                {
                    if (sub[1] == user.username)
                    {
                        byte[] data = Encoding.ASCII.GetBytes("  Name is already in use");
                        currentClientSocket.socket.Send(data);
                        nameUsed = true;
                        break;
                    }
                    else if (nameUsed == false)
                    {
                        SendToAll(" User: " + currentClientSocket.username + " is changing name to " + sub[1], currentClientSocket);
                        AddToChat("User: " + currentClientSocket.username + " is changing name to " + sub[1]);
                        currentClientSocket.username = sub[1];
                        break;

                    }

                }
            }
            // Send dm to persons on the server 
            else if (sub[0] == "!whisper")
            {
                bool userFound = false;
                //Loop analyzes if the receiver exits
                foreach (ClientSocket secret in clientSockets)
                {
                    if (sub[1] == secret.username)
                    { // send message only to the client socket with the appropiate username
                        byte[] dataS = Encoding.ASCII.GetBytes(currentClientSocket.username + ": " + text);
                        secret.socket.Send(dataS);
                        currentClientSocket.socket.Send(dataS);
                        userFound = true;

                    }
                }
                // username not found
                if (userFound == false)
                {
                    byte[] dataSecretError = Encoding.ASCII.GetBytes("User was not found");
                    currentClientSocket.socket.Send(dataSecretError);
                }
            }
            // moderator be promoted or demoted
            else if (sub[0] == "!mod")
            {
                foreach (ClientSocket mod in clientSockets)
                {
                    if (sub[1] == mod.username && mod.mod == false)
                    {
                        mod.mod = true;
                        AddToChat(mod.username + " was promoted to moderator " + mod.mod);

                    }
                    else if (sub[1] == mod.username && mod.mod == true)
                    {
                        mod.mod = false;
                        AddToChat(mod.username + " was demoted from being a moderator" + mod.mod);
                    }

                }
            }
            else if (text.ToLower() == "!commands") // Client requested time
            {
                byte[] data = Encoding.ASCII.GetBytes("Commands are !commands !about !who !whisper !exit ");
                currentClientSocket.socket.Send(data);
                AddToChat("Commands sent to client");
            }
            else if (text.ToLower() == "!about") // Client wants to exit gracefully
            {
                byte[] data = Encoding.ASCII.GetBytes("Chat Parangaritcuitimicuaro, made by Billy Blue College NDS Class 2022 and the purpose is that Matt wanted to torture their students with a very hard assessment\n");
                currentClientSocket.socket.Send(data);
                AddToChat("Information sent to client");
            }
            else if (text.ToLower() == "!who") // Client wants to know whos in the chat
            {
                byte[] dataW = Encoding.ASCII.GetBytes("List of current users");
                currentClientSocket.socket.Send(dataW);
                foreach (ClientSocket clientSocket in clientSockets)
                {
                    byte[] data = Encoding.ASCII.GetBytes(clientSocket.username + ", \n");
                    currentClientSocket.socket.Send(data);
                }

            }
            else if (text.ToLower() == "!loney")// Client is loney someone do something
            {
                SendToAll(currentClientSocket.username + ": I'm very loney does someone wants to come over;)", currentClientSocket);
                SendToAll("Siri: Nope go we don't do that here", currentClientSocket);
            }
            else if (sub[0] == "!kick" && currentClientSocket.mod == true) // kick people out if you are a moderator
            {
                foreach (ClientSocket clientSocket in clientSockets)
                {
                    if (clientSocket.username == sub[1])
                    {
                        byte[] dataK = Encoding.ASCII.GetBytes("User: " + clientSocket.username + " was kick out");
                        currentClientSocket.socket.Send(dataK);
                        clientSocket.socket.Shutdown(SocketShutdown.Both);
                        clientSocket.socket.Disconnect(true);
                        clientSockets.Remove(clientSocket);
                        AddToChat("Client kick out");
                    }
                }
            }
            else if (text.ToLower() == "!exit") // Client wants to exit gracefully
            {
                // Always Shutdown before closing
                currentClientSocket.socket.Shutdown(SocketShutdown.Both);
                currentClientSocket.socket.Close();
                clientSockets.Remove(currentClientSocket);
                AddToChat("Client disconnected");
                return;
            }
            else if (text.ToLower() == "!join")
            {
                bool findplayer1 = false;
                bool findplayer2 = false;
                currentClientSocket.state = clientState.playing;
                foreach (ClientSocket game in clientSockets)
                {
                    if (game.pstate == playerState.player1)
                    {
                        findplayer1 = true;
                    }
                    else if (game.pstate == playerState.player2)
                    {
                        findplayer2 = true;
                    }

                }
                if (findplayer1 == false)
                {
                    byte[] data = Encoding.ASCII.GetBytes("!player1");
                    currentClientSocket.socket.Send(data);
                    currentClientSocket.pstate = playerState.player1;
                    //currentClientSocket.yourTurn = true;

                    byte[] dataT = Encoding.ASCII.GetBytes("!your turn");
                    currentClientSocket.socket.Send(dataT);

                }
                else if (findplayer2 == false)
                {
                    byte[] data = Encoding.ASCII.GetBytes("!player2");
                    currentClientSocket.socket.Send(data);
                    currentClientSocket.pstate = playerState.player2;
                    findplayer2 = true;
                    SendToAll("Players selected thank you all! ", currentClientSocket);
                }

                //else if (text.ToLower() == "!move")
                //else
                // GameState full
            }
            else if (sub[0] == "!move")
            {
                int i = Int16.Parse(sub[1]);

                ticTacToe.SetTile(i, ticTacToe.playerTileType);
                //!Not your turn send to client .yourturn is false and not only server knows about this 
                currentClientSocket.yourTurn =false;
                foreach(ClientSocket turn in clientSockets)
                {
                    if(currentClientSocket.pstate== playerState.player1 && turn.pstate== playerState.player2)
                    {
                        byte[] data = Encoding.ASCII.GetBytes("!your turn");
                        turn.socket.Send(data);

                    }
                    if (currentClientSocket.pstate == playerState.player2 && turn.pstate == playerState.player1)
                    {
                        byte[] data = Encoding.ASCII.GetBytes("!your turn");
                        turn.socket.Send(data);
                    }
                    //if clientSocket is player 1 annd turn is player 2, tell player 2 its theirn turn
                    //else if client socket is player 2 and turn is player 1, tell player 1 its their turn
                }


            }
            else
            {
                //normal message broadcast out to all clients
                SendToAll(currentClientSocket.username+": "+ text, currentClientSocket);
            }
            //we just received a message from this socket, better keep an ear out with another thread for the next one
            currentClientSocket.socket.BeginReceive(currentClientSocket.buffer, 0, ClientSocket.BUFFER_SIZE, SocketFlags.None, ReceiveCallback, currentClientSocket);
        }

        public void SendToAll(string str, ClientSocket from)
        {
            foreach(ClientSocket c in clientSockets)
            {
                if(from == null || !from.socket.Equals(c))
                {
                    byte[] data = Encoding.ASCII.GetBytes(str);
                    c.socket.Send(data);
                }
            }
        }

        
    }
}
