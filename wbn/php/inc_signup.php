<?
/*
 * $Id$
 *
 * Copyright (c) 1998-2008 John Morrison.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
?>

<table width="480" border="0" align="right" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
<tr>
<td> <table width="480" border="0" cellspacing="0" cellpadding="0">
<tr>
<td width="20"><img src="images/stleft.png" width="20" height="20"></td>
<td class="topborder" bgcolor="#3366CC"> <div class="heading">
Login/Signup
</div></td>
<td width="20"><img src="images/stright.png" width="20" height="20"></td>
</tr>
</table></td>
</tr>
<tr>
<td> <table width="480" border="0" cellspacing="0" cellpadding="0">
<tr>
<td width="1" class="leftborder">&nbsp;</td>
<td width="282">
<table width="470" border="0" align="center" cellpadding="0" cellspacing="0">
<tr>
</tr>
<tr>
<td> <div class="subheading">
Existing Accounts</div></td></tr>
<tr><td><div class="bodytext">
<p><font size=2><span class="normal">If you already have an account 
sign in </span></font></p>
<form method="post" action="login.php">
Player Name: 
<input type="text" name="username" size="10">
Password:
<input type="password" name="password" size="10">
<input type="submit" name="login" value="Log on">
<BR>
<input type="checkbox" name="autologin" size=10>Automatic login each visit
</span> 
            </form>
            <p><span class="normal">Forgotten your password? <span class="normal">Request a new one <a href="lostpassword.php">here</a>
</div></td></tr>
<tr><td><div class="subheading">New Player Signup</div></td></tr>
<tr><td><div class="bodytext">
            <p>To activate your account please fill out the 
              following form</span></p>

</font> 
                    <form method="post" action="processsignup.php">
                      <p><b>Required Information</b></p>
                      <p>Once your account is created most of this 
                        information can be modified from the modify account page.</p>
                      <table width="90%" border="1" align="center" bordercolor="#333333" class="bodytext">
                        <tr> 
                          <td>Winbolo.net login</td>
                          <td> 
                            <input type="text" name="login" maxlength="32" size="20">
                          </td>
                          <td>Your requested logon 
                            name</td>
                        </tr>
                        <tr> 
                          <td>Full Name</td>
                          <td>
                            <input type="text" name="fullname" maxlength="255" size="20">
                          </td>
                          <td>Your name</td>
                        </tr>
                        <tr> 
                          <td>Password</td>
                          <td> 
                            <input type="password" name="password" size="20" maxlength="32">
                          </td>
                          <td>Your account password</td>
                        </tr>
                        <tr> 
                          <td>Re-enter Password</td>
                          <td> 
                            <input type="password" name="password2" size="20" maxlength="32">
                          </td>
                          <td>Your account password</td>
                        </tr>
                        <tr> 
                          <td>Email Address</td>
                          <td> 
                            <input type="text" name="email" size="20" maxlength="255">
                          </td>
                          <td>Your email address</td>
                        </tr>
			<tr>
			<td class="row1" colspan="3" align="center">If you are visually impaired or cannot otherwise read this code please contact the Administrator for help.</span><br /><br /><img src="/forums/profile.php?mode=confirm&amp;id=<?echo $confirm_id;?>" alt="" title="" /><br /><br /></td>
			</tr>
                          <td>Confirmation code</td>
                          <td> 
                            <input type="text" name="confirm_code" size="20" maxlength="255">
                          </td>
                          <td>Enter the code exactly as you see it. The code is case sensitive and zero has a diagonal line through it.</td>
			  </tr><tr>
			  <td>What is the current version of WinBolo?</td><td><input type="text" name="confirm_version" size="20" maxlength="10"></td><td>The about box of WinBolo will tell you the current version. As will the downloads page.</td>
			</tr>
                      </table>
		      <p>
		      <b><font color='#FF0000'>Please note to enable your winbolo.net email alias your login must only contain letters and numbers. You will not be able to enable your winbolo.net email alias if your login does not conform to this.</font></b>
                      <p>&nbsp;</p>
                      <p><b>Optional Information</b></p>
                      <table class="bodytext" width="95%" border="1">
                        <tr> 
                          <td>Country</td>
                          <td> 
                            <select name=country size=1>
                              <option selected>Not Telling</option>
                              <option >United States of America </option>
                              <option>Afghanistan </option>
                              <option>Albania </option>
                              <option>Algeria </option>
                              <option>Andorra </option>
                              <option>Angola </option>
                              <option>Anguilla </option>
                              <option>Antigua &amp; Barbuda </option>
                              <option>Argentina </option>
                              <option>Armenia </option>
                              <option>Aruba </option>
                              <option>Ascension </option>
                              <option>Australia </option>
                              <option>Austria </option>
                              <option>Azerbaijan </option>
                              <option>Azores </option>
                              <option>Bahamas </option>
                              <option>Bahrain </option>
                              <option>Bangladesh </option>
                              <option>Barbados </option>
                              <option>Belgium </option>
                              <option>Belize </option>
                              <option>Benin </option>
                              <option>Bermuda </option>
                              <option>Bhutan </option>
                              <option>Bolivia </option>
                              <option>Bosnia (Hercgovina) </option>
                              <option>Botswana </option>
                              <option>Brazil </option>
                              <option>British Virgin Islands </option>
                              <option>Brunei </option>
                              <option>Bulgaria </option>
                              <option>Burkina Faso </option>
                              <option>Burma </option>
                              <option>Burundi </option>
                              <option>Byelorussia (Belarus) </option>
                              <option>Cameroon </option>
                              <option>Canada </option>
                              <option>Canary Islands </option>
                              <option>Cape Verde </option>
                              <option>Cayman Islands </option>
                              <option>Central African Republic </option>
                              <option>Chad </option>
                              <option>Chile </option>
                              <option>China </option>
                              <option>Colombia </option>
                              <option>Comoros </option>
                              <option>Congo </option>
                              <option>Corsica </option>
                              <option>Costa Rica </option>
                              <option>Cote d'Ivoire </option>
                              <option>Croatia </option>
                              <option>Cuba </option>
                              <option>Cyprus </option>
                              <option>Czech Republic </option>
                              <option>Czechoslovakia </option>
                              <option>Denmark </option>
                              <option>Djibouti </option>
                              <option>Dominica </option>
                              <option>Dominican Rep. </option>
                              <option>East Timor </option>
                              <option>Ecuador </option>
                              <option>Egypt </option>
                              <option>El Salvador </option>
                              <option>Equatorial Guinea </option>
                              <option>Estonia </option>
                              <option>Ethiopia </option>
                              <option>Falkland Islands </option>
                              <option>Faroe Islands </option>
                              <option>Fiji </option>
                              <option>Finland </option>
                              <option>France </option>
                              <option>French Guiana </option>
                              <option>French Polynesia </option>
                              <option>Gabon </option>
                              <option>Gambia </option>
                              <option>Germany </option>
                              <option>Ghana </option>
                              <option>Gibraltar </option>
                              <option>Great Britain </option>
                              <option>Greece </option>
                              <option>Greenland </option>
                              <option>Grenada </option>
                              <option>Guadeloupe </option>
                              <option>Guatemala </option>
                              <option>Guinea </option>
                              <option>Guinea-Bissau </option>
                              <option>Guyana </option>
                              <option>Haiti </option>
                              <option>Honduras </option>
                              <option>Hong Kong </option>
                              <option>Hungary </option>
                              <option>Iceland </option>
                              <option>India </option>
                              <option>Indonesia </option>
                              <option>Iran </option>
                              <option>Iraq </option>
                              <option>Ireland (Eire) </option>
                              <option>Israel </option>
                              <option>Italy </option>
                              <option>Jamaica </option>
                              <option>Japan </option>
                              <option>Jordan </option>
                              <option>Kampuchea </option>
                              <option>Kazakhstan </option>
                              <option>Kenya </option>
                              <option>Kirghizia (Kyrgyzstan) </option>
                              <option>Kiribati </option>
                              <option>Korea, Democ. Peoples Rep. </option>
                              <option>Kuwait, Republic of </option>
                              <option>La Luia </option>
                              <option>Laos </option>
                              <option>Latvia </option>
                              <option>Lebanon </option>
                              <option>Lesotho </option>
                              <option>Liberia </option>
                              <option>Libya </option>
                              <option>Liechtenstein </option>
                              <option>Lithuania </option>
                              <option>Luxembourg </option>
                              <option>Macau </option>
                              <option>Macedonia </option>
                              <option>Madagascar </option>
                              <option>Madeira Islands </option>
                              <option>Malawi </option>
                              <option>Malaysia </option>
                              <option>Maldives </option>
                              <option>Mali </option>
                              <option>Malta </option>
                              <option>Martinique </option>
                              <option>Mauritania </option>
                              <option>Mauritius </option>
                              <option>Mexico </option>
                              <option>Moldova </option>
                              <option>Monaco </option>
                              <option>Mongolia </option>
                              <option>Montserrat </option>
                              <option>Morocco </option>
                              <option>Mozambique </option>
                              <option>Nauru </option>
                              <option>Nepal </option>
                              <option>Netherlands </option>
                              <option>Netherlands Antilies </option>
                              <option>New Caledonia </option>
                              <option>New Zealand </option>
                              <option>Nicaragua </option>
                              <option>Niger </option>
                              <option>Nigeria </option>
                              <option>Norway </option>
                              <option>Oman </option>
                              <option>Pakistan </option>
                              <option>Panama </option>
                              <option>Papua New Guinea </option>
                              <option>Paraguay </option>
                              <option>Peru </option>
                              <option>Philippines </option>
                              <option>Pitcairn Islands </option>
                              <option>Poland </option>
                              <option>Portugal </option>
                              <option>Qatar </option>
                              <option>Republic of Georgia </option>
                              <option>Reunion </option>
                              <option>Romania </option>
                              <option>Russia </option>
                              <option>Rwanda </option>
                              <option>Saint Helena </option>
                              <option>Saint Lucia </option>
                              <option>Saint Pierre &amp; Miquelon </option>
                              <option>San Marino </option>
                              <option>Sao Tome &amp; Principe </option>
                              <option>Saudi Arabia </option>
                              <option>Scotland </option>
                              <option>Senegal </option>
                              <option>Servia (Montenegro) </option>
                              <option>Seychelles </option>
                              <option>Sierra Leone </option>
                              <option>Singapore </option>
                              <option>Slovakia </option>
                              <option>Slovenia </option>
                              <option>Solomon Islands </option>
                              <option>Somalia </option>
                              <option>South Africa </option>
                              <option>South Korea </option>
                              <option>Spain </option>
                              <option>Sri Lanka </option>
                              <option>St. Christopher &amp; Nevis </option>
                              <option>St. Vincent? Grenadines </option>
                              <option>Sudan </option>
                              <option>Suriname </option>
                              <option>Swaziland </option>
                              <option>Sweden </option>
                              <option>Switzerland </option>
                              <option>Syria </option>
                              <option>Tadzhikistan </option>
                              <option>Taiwan </option>
                              <option>Tanzania </option>
                              <option>Thailand </option>
                              <option>Togo </option>
                              <option>Tonga </option>
                              <option>Trinidad and Tobago </option>
                              <option>Tristan da Cunha </option>
                              <option>Tunisia </option>
                              <option>Turkey </option>
                              <option>Turkmenistan </option>
                              <option>Turks &amp; Caicos Islands </option>
                              <option>Tuvalu </option>
                              <option>U.S.S.R. </option>
                              <option>Uganda </option>
                              <option>Ukraine </option>
                              <option>United Arab Emirates </option>
                              <option>United States of America </option>
                              <option>Uruguay </option>
                              <option>Uzbekistan </option>
                              <option>Vanuatu </option>
                              <option>Vatican City </option>
                              <option>Venezuela </option>
                              <option>Vietnam </option>
                              <option>Wallis &amp; Futuna Islands </option>
                              <option>Western Samoa </option>
                              <option>Yemen </option>
                              <option>Yugoslavia </option>
                              <option>Zaire </option>
                              <option>Zambia </option>
                              <option>Zimbabwe</option>
                            </select>
                          </td>
                        </tr>
                        <tr> 
                          <td>Prefered Bolo Platform:</td>
                          <td>
			  <select name=platform size=1>
                              <option value=0 selected>Not Telling</option>
                              <option value=1>Windows</option>
                              <option value=2>Linux</option>
			  </select></td>
                        </tr>
                        <tr> 
                          <td>Favourite Bolo Game Type 
                          </td>
                          <td> 
                            <select name="gametype">
                              <option value="0">No Preference</option>
                              <option value="1">Open</option>
                              <option value="2">Tournament</option>
                              <option value="3">Strict</option>
                            </select>
                          </td>
                        </tr>
                        <tr> 
                          <td>Blurb</td>
                          <td> 
                            <textarea name="blurb" cols="30" rows="6"></textarea>
                          </td>
                        </tr>
                        <tr> 
                          <td>Show Email Address on 
                            page</td>
                          <td> 
                            <input type="radio" name="showemail" value="1">
                            Yes 
                            <input type="radio" name="showemail" value="0" checked>
                            No </td>
                        </tr>
                        <tr> 
                          <td>Show blurb on page</td>
                          <td> 
                            <input type="radio" name="showblurb" value="1">
                            Yes 
                            <input type="radio" name="showblurb" value="0" checked>
                            No </td>
                        </tr>

                        <tr> 
<td><img src="/images/icq.gif"> ICQ Number</td>
<td><input type="text" name="icq" size=30></td></tr>
<tr><td><img src="/images/aim.gif"> AIM</td>
<td><input type="text" name="aim" size=30></td></tr>
<tr><td><img src="/images/yahoo.gif"> Yahoo Messenger</td>
<td><input type="text" name="yahoo" size=30></td></tr>
<tr><td><img src="/images/msn.gif"> MSN</td>
<td><input type="text" name="msn" size=30></td></tr>
<tr><td>Website Address</td>
<td><input type="text" name="website" size=30></td></tr>
<tr><td>Occupation</td>
<td><input type="text" name="occupation" size=30></td></tr>
<tr><td>Signature<br><font size=-1>Text that appears at the end of your posts. Max 255 letters.</font></td>
<td><TEXTAREA NAME="sig" ROWS="4" COLS="30"></textarea></td>
			</tr>
		    </table>
                      <p align="right">
                        <input type="hidden" name="confirm_id" value="<?echo $confirm_id;?>" />
			<input type="submit" name="Submit2" value="Create Account">
                      </p>
                      <p align="left" class="bodytext">By signing up you agree to <a href="legal.php">Winbolo.net Terms of Use</a>. I have also read and understood the <a href="/help.php">help</a> pages which describe how ranking works.</p>
                    </form>
<p>&nbsp;</p>
</div></td></tr>
</table>
</td></tr>
</table>
