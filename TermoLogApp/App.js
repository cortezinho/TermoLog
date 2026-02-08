import React, { useState, useEffect } from 'react';
import { StyleSheet, Text, View, TouchableOpacity, ScrollView, TextInput, SafeAreaView, ActivityIndicator } from 'react-native';

export default function App() {
  const [temp, setTemp] = useState("--");
  const [umid, setUmid] = useState("--");
  const [espIp, setEspIp] = useState(""); 
  const [historico, setHistorico] = useState([]);
  const [isMonitoring, setIsMonitoring] = useState(false);

  useEffect(() => {
    let interval;
    if (isMonitoring && espIp) {
      interval = setInterval(fetchDados, 3000);
    }
    return () => clearInterval(interval);
  }, [isMonitoring, espIp]);

  const fetchDados = async () => {
    try {
      const response = await fetch(`http://${espIp}/data`);
      const json = await response.json();
      setTemp(json.temp);
      setUmid(json.umid);
      setHistorico(prev => [{id: Date.now().toString(), hora: new Date().toLocaleTimeString(), t: json.temp, u: json.umid}, ...prev].slice(0, 20));
    } catch (error) {
      console.log("Erro de conexão. Verifique o IP!");
    }
  };

  return (
    <SafeAreaView style={styles.container}>
      <Text style={styles.header}>TermoLog Wi-Fi</Text>
      
      <View style={styles.ipCard}>
        <Text style={styles.label}>DIGITE O IP QUE APARECE NO PC:</Text>
        <TextInput 
          style={styles.input}
          value={espIp}
          onChangeText={setEspIp}
          placeholder="Ex: 192.168.1.15"
          keyboardType="numeric"
        />
        <TouchableOpacity 
          style={[styles.btn, {backgroundColor: isMonitoring ? '#f44336' : '#2196f3'}]} 
          onPress={() => setIsMonitoring(!isMonitoring)}
        >
          <Text style={styles.btnText}>{isMonitoring ? "PARAR" : "CONECTAR"}</Text>
        </TouchableOpacity>
      </View>

      <View style={styles.row}>
        <View style={styles.card}><Text style={styles.cardLabel}>TEMP</Text><Text style={styles.value}>{temp}°C</Text></View>
        <View style={styles.card}><Text style={styles.cardLabel}>UMID</Text><Text style={styles.value}>{umid}%</Text></View>
      </View>

      <ScrollView style={styles.history}>
        {historico.map(item => (
          <View key={item.id} style={styles.logRow}>
            <Text>{item.hora}</Text>
            <Text style={{fontWeight: 'bold'}}>{item.t}°C | {item.u}%</Text>
          </View>
        ))}
      </ScrollView>
    </SafeAreaView>
  );
}

const styles = StyleSheet.create({
  container: { flex: 1, backgroundColor: '#f0f2f5', padding: 20 },
  header: { fontSize: 24, fontWeight: 'bold', textAlign: 'center', marginTop: 50, marginBottom: 20 },
  ipCard: { backgroundColor: '#fff', padding: 20, borderRadius: 15, elevation: 5, marginBottom: 20 },
  label: { fontSize: 10, color: '#888', marginBottom: 5 },
  input: { borderBottomWidth: 2, borderBottomColor: '#2196f3', fontSize: 18, marginBottom: 15, padding: 5 },
  btn: { padding: 15, borderRadius: 10, alignItems: 'center' },
  btnText: { color: '#fff', fontWeight: 'bold' },
  row: { flexDirection: 'row', justifyContent: 'space-between', marginBottom: 20 },
  card: { backgroundColor: '#fff', width: '48%', padding: 20, borderRadius: 15, alignItems: 'center', elevation: 3 },
  cardLabel: { fontSize: 10, color: '#888' },
  value: { fontSize: 32, fontWeight: 'bold' },
  history: { flex: 1 },
  logRow: { flexDirection: 'row', justifyContent: 'space-between', padding: 10, backgroundColor: '#fff', marginBottom: 5, borderRadius: 8 }
});